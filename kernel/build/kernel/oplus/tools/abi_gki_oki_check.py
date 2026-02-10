import os
import subprocess
import time
import shutil
import fnmatch
import pathlib
import sys
import csv
import codecs
import io
import glob
from datetime import datetime

out_put = "out/oplus"
log_dir= 'LOGDIR/abi/'
exclude_file = ['*.ko', 'vmlinux']

def set_env_from_file(filepath):
    with open(filepath, 'r') as file:
        for line in file:
            # Remove any whitespace and newline characters, then check if the line contains environment variable definitions
            line = line.strip()
            if line and not line.startswith('#'):  # Ignore blank lines and comments
                key, _, value = line.partition('=')
                if key and value:
                    # Set the environment variable in the current process's environment
                    os.environ[key] = value
                    # Print the variable (if required)
                    #print("Set {}={}".format(key, value))

def delete_all_exists_file(folder_path):
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)

    for item in os.listdir(folder_path):
        item_path = os.path.join(folder_path, item)
        if os.path.isdir(item_path):
            shutil.rmtree(item_path)
        else:
            os.remove(item_path)

def create_symlinks_from_pairs(src_and_targets):
    for src_pattern, target_filename in src_and_targets:
        src_files = glob.glob(src_pattern)

        if len(src_files) == 1:
            src_filename = src_files[0]

            if not os.path.exists(src_filename):
                print("Source file does not exist: {}".format(src_filename))
                continue

            src_absolute_path = pathlib.Path(src_filename).resolve()  # obtain absolute path
            if os.path.exists(target_filename):
                os.remove(target_filename)
            os.symlink(str(src_absolute_path), target_filename)
        elif len(src_files) > 1:
            print("Multiple source files match the pattern '{}'. Cannot create symlink.".format(src_pattern))
        else:
            print("No source files match the pattern '{}'. Cannot create symlink.".format(src_pattern))

def link_files_from_directories(src_dir_list, file_pattern, target_dir):
    target_path = pathlib.Path(target_dir)
    target_path.mkdir(parents=True, exist_ok=True)
    for src_dir in src_dir_list:
        src_path = pathlib.Path(src_dir)
        for file in src_path.rglob(file_pattern):
            file_absolute_path = file.resolve()  # obtain absolute path
            symlink_target = target_path / file.name
            if symlink_target.exists():
                os.remove(str(symlink_target))
            os.symlink(str(file_absolute_path), str(symlink_target))

def generate_abi(output_dir):
    generate_abi_gki_aarch64_oplus = "kernel/build/abi/extract_symbols {}/ "\
                                             "--skip-module-grouping --symbol-list "\
                                             "{}/abi_gki_aarch64_oplus "\
                                             "| grep 'Symbol '>> "\
                                             "{}/abi_required_full.txt".format(output_dir, output_dir, output_dir)
    process = subprocess.Popen(generate_abi_gki_aarch64_oplus, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    #print("stand output:{}".format(stdout.decode('utf-8')))
    #print("stand error:{}".format(stderr.decode('utf-8')))

def get_symbols_and_ko_list(input_file, output_list, output_ko, output_symbol_ko, delimiter=' '):
    unique_symbols = set()
    unique_ko = set()
    unique_symbols_ko = set()
    with open(input_file, 'r') as f_in:
        for line in f_in:
            columns = line.strip().split(delimiter)
            if len(columns) >= 4:
                unique_symbols.add(columns[1])
                unique_ko.add(columns[4])
                entry = (columns[1], columns[4])
                unique_symbols_ko.add(entry)

    with open(output_list, 'w') as f_out:
        #print("missing symbols is:")
        for item in unique_symbols:
            #print(item)
            f_out.write(item + '\n')

    with open(output_ko, 'w') as ko_out:
        #print("symbols is needed by:")
        for item in unique_ko:
            #print(item)
            ko_out.write(item + '\n')

    with open(output_symbol_ko, 'w') as s_ko_out:
        for symbol, ko in unique_symbols_ko:
            s_ko_out.write(symbol +' ' + ko + "\n")

def get_weak_symbols_and_compare(input_file, output_ko_names, tmp_approval_file):
    written_symbols = set()
    written_kos = set()

    with open(input_file, 'r') as f_in:
        for line in f_in:
            parts = line.strip().split()
            if len(parts) != 2:
                print("Error: Input line doesn't have exactly two parts: {}".format(line))
                continue

            symbol, ko = parts
            #print("Processing: {} in {}".format(symbol, ko))

            try:
                process = subprocess.Popen(["nm", out_put + "/" + ko], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                nm_output, _ = process.communicate()
                nm_output = nm_output.decode()
                # Checking nm output for weak symbols
                for nm_line in nm_output.splitlines():
                    nm_line_parts = nm_line.strip().split()
                    if len(nm_line_parts) == 3 and nm_line_parts[-1] == symbol and nm_line_parts[1] in ("W", "w"):
                        print("Weak symbol found in 3 columns format: {}".format(nm_line))
                        if symbol not in written_symbols:
                            with open(tmp_approval_file, 'a') as f_approval:
                                f_approval.write(symbol + '\n')
                                written_symbols.add(symbol)
                        if ko not in written_kos:
                            with open(output_ko_names, 'a') as f_ko_names:
                                f_ko_names.write(ko + '\n')
                                written_kos.add(ko)
                        break
                    elif len(nm_line_parts) == 2 and nm_line_parts[-1] == symbol and nm_line_parts[0] in ("W", "w"):
                        print("Weak symbol found in 2 columns format:\n {}".format(nm_line))
                        if symbol not in written_symbols:
                            with open(tmp_approval_file, 'a') as f_approval:
                                f_approval.write(symbol + '\n')
                                written_symbols.add(symbol)
                        if ko not in written_kos:
                            with open(output_ko_names, 'a') as f_ko_names:
                                f_ko_names.write(ko + '\n')
                                written_kos.add(ko)
                        break

            except subprocess.CalledProcessError as e:
                print("Error running nm on {}: {}".format(ko, e))
            except OSError as e:
                print("OS error encountered: {}".format(e))

def remove_lines_and_write_new_file(required_file, weak_file, new_file):
    if not os.path.exists(required_file):
        print("Error: The file {} does not exist.".format(required_file))
        return

    with open(required_file, 'r') as file:
        required_lines = set(file.read().splitlines())

    if not os.path.exists(weak_file):
        with open(new_file, 'w') as file:
            for line in sorted(required_lines):
                file.write(line + '\n')
        return

    with open(weak_file, 'r') as file:
        weak_lines = set(file.read().splitlines())

    updated_lines = required_lines - weak_lines

    with open(new_file, 'w') as file:
        for line in sorted(updated_lines):
            file.write(line + '\n')

def get_approval_config_from_file(input_file: str, output_file: str) -> None:
    # read content from CSV file
    with open(input_file, "r", encoding='utf-8') as file:
        csv_reader = csv.reader(file, delimiter=',')
        first_column = [row[0] for row in csv_reader if row and row[0].strip()]

    # determine if the data is valid
    if not first_column:
        print("No valid data found in {}.".format(input_file))
        with open(output_file, 'w') as file:
             pass
        return

    # temporarily store the extracted content in a StringIO object
    output_buffer = io.StringIO()
    for item in first_column:
        output_buffer.write("{}\n".format(item))

    output_without_bom = output_buffer.getvalue().replace('\ufeff', '')

    # write the extracted content into a new file
    with open(output_file, "w", encoding='utf-8') as output:
        output.write(output_without_bom)

def compare_and_output_extra_lines(base_file, new_file, output_file):
    # read the content of basic files and new files
    with open(base_file, 'r', encoding='utf-8') as f_base:
        base_lines = f_base.readlines()

    with open(new_file, 'r', encoding='utf-8') as f_new:
        new_lines = f_new.readlines()

    extra_lines = []
    for new_line in new_lines:
        if new_line not in base_lines:
            extra_lines.append(new_line)

    #print("Extra Lines:")
    for line in extra_lines:
        print(line)

    # write the excess number of lines to the specified file
    with open(output_file, 'w') as f_output:
        for line in extra_lines:
            f_output.write("{}".format(line))

def compare_same_line_files(base_file, new_file, output_file):
    with open(base_file, "r") as file1, open(new_file, "r") as file2:
        base_lines = file1.readlines()
        new_lines = file2.readlines()

    with open(output_file, "w") as output:
        for line in new_lines:
            if line in base_lines:
                output.write(line)
                print("missing symbols in recovery mode ko:\n{}".format(line.strip()))

def copy_log_files_to_dest(src, dest, exclude_patterns=None):
    print("\nCopy build log from {} to {} exclude{}".format(src,dest,exclude_patterns))
    print("You can find log in {} or in {} ".format(src,dest))
    print("More information for help please read doc link: ")
    print("https://odocs.myoas.com/docs/Wr3DVZ8l9XumadkJ/ \n")

    if not os.path.exists(src):
        raise ValueError("src dir {src} not exists".format(file_path))

    if not os.path.exists(dest):
        os.makedirs(dest)

    for item in os.listdir(src):
        src_item = os.path.join(src, item)
        dest_item = os.path.join(dest, item)

        if exclude_patterns and any(fnmatch.fnmatch(item, pattern) for pattern in exclude_patterns):
            continue

        if os.path.isdir(src_item):
            copy_files(src_item, dest_item, exclude_patterns)
        else:
            shutil.copy2(src_item, dest_item)

def check_file_is_empty(file_path):
    file = pathlib.Path(file_path)
    if not file.exists():
        raise FileNotFoundError("File not found: {}".format(file_path))

    if file.stat().st_size != 0:
        with file.open('r') as f:
            content = f.read()
        print("\nFile {} content:\n{}".format(file_path, content))
        raise Exception("File {} is not null please check reason".format(file_path))

def find_ko_files(path):
    ko_files = []
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith('.ko'):
                ko_files.append(os.path.join(root, file))
    return ko_files

def dump_modversions(ko_files):
    ko_files.sort()
    for ko_file in ko_files:
        modprobe_cmd = "modprobe --dump-modversions {} >> {}/ko_modversions_full.txt".format(ko_file, out_put)
        os.system(modprobe_cmd)
    return ko_files

def sort_by_second_column(line):
    columns = line.split()
    return columns[1]

def remove_duplicates_and_sort_by_second_column(input_file, output_file):
    with open(input_file, "r") as file:
        lines = file.readlines()
    unique_lines = list(set(lines))
    unique_lines.sort(key=sort_by_second_column)
    with open(output_file, "w") as output_file:
        for line in unique_lines:
            output_file.write(line)

def tab_to_spaces(input_file, output_file):
    with open(input_file, "r") as file:
        content = file.read()
    content = content.replace("\t", " ")
    with open(output_file, "w") as output:
        output.write(content)

def generate_modversions_from_src_to_dst(input_file, output_file):
    # read content from file
    with open(input_file, "r") as file:
        lines = file.readlines()

    columns = [line.split()[:2] for line in lines if line.strip() and len(line.split()) >= 2]

    # determine if the data is valid
    if not columns:
        print("No valid data found in {}.".format(out_put))
        return

    # write the extracted content into a new file
    with open(output_file, "w") as output:
        for col1, col2 in columns:
            output.write("{} {}\n".format(col1,col2))

def compare_symbols_modversions(current_file, base_file, output_file):
    with open(current_file, "r") as f1:
        data1 = [line.split() for line in f1]
        data1 = [(item[0], item[1]) for item in data1 if len(item) == 2]

    with open(base_file, "r") as f2:
        data2 = [line.split() for line in f2]
        data2 = [(item[0], item[1]) for item in data2 if len(item) == 2]

    with open(output_file, "w") as output:
        for item1 in data1:
            for item2 in data2:
                if item1[1] == item2[1] and item1[0] != item2[0]:
                    result_line = "{}: {} {}\n{}: {} {}\n".format(current_file, item1[0], item1[1], base_file, item2[0], item2[1])
                    output.write(result_line)
                    print(result_line)

def format_duration(td):
    minutes, seconds = divmod(td.seconds, 60)
    hours, minutes = divmod(minutes, 60)
    return '{:02} Hour {:02} Minute {:02} Second'.format(hours, minutes, seconds)

def main():
    start_time = datetime.now()
    print("Begin time:", start_time.strftime("%Y-%m-%d %H:%M:%S"))
    print("abi_gki_oki_check.py start ...")

    os.system("cd ../../../")
    if not os.path.exists("out_krn"):
        print("dir out_krn not exist, this is before 5.10 version, ignore")
        sys.exit(0)

    #set_env_from_file('kernel/kerne-5.10/build.config.common')
    #print("clang version:" + os.environ.get('CLANG_VERSION'))
    CLANG_PREBUILT_BIN = "kernel/prebuilts-master/clang/host/linux-x86/clang-r416183b/bin:"
    print("clang path is:" + CLANG_PREBUILT_BIN)
    os.environ["PATH"] = "kernel/build/build-tools/path/linux-x86:" + os.environ["PATH"]
    os.environ["PATH"] = CLANG_PREBUILT_BIN + os.environ["PATH"]

    print("[1/15] delete_all_exists_file...")
    delete_all_exists_file(out_put)

    # example: Predefining source and target files
    src_and_targets = [
        ('vendor/aosp_gki/kernel-5.10/aarch64/vmlinux.symvers', 'out/oplus/vmlinux_gki.symvers'),
        ('out_krn/target/product/vnd/obj/KERNEL_OBJ/kernel-5.10/vmlinux.symvers', 'out/oplus/vmlinux.symvers'),
        ('vendor/aosp_gki/kernel-5.10/aarch64/vmlinux', 'out/oplus/vmlinux'),
        ('kernel/build/kernel/oplus/config/abi_symbols_tmp_approval.csv', 'out/oplus/abi_symbols_tmp_approval.csv'),
        ('out/target/product/vnd/vendor_ramdisk/lib/modules/modules.load.recovery', 'out/oplus/modules.load.recovery')
    ]
    print("[2/15] create_symlinks for vmlinx and config file...")
    create_symlinks_from_pairs(src_and_targets)

    print("[3/15] create_symlinks_for_all_ko...")
    src_dir_list = [
        pathlib.Path("out_krn/target/product/vnd/symbols/kernel-5.10/system/lib/modules/"),
    ]
    target_dir = pathlib.Path(out_put)
    link_files_from_directories(src_dir_list, "*.ko", target_dir)

    print("[4/15] generate_abi...")
    generate_abi(out_put)

    print("[5/15] get_symbols_and_ko_list...")
    file_path = "{}/abi_required_full.txt".format(out_put)
    output_list = "{}/abi_required_symbol.txt".format(out_put)
    output_ko = "{}/abi_required_ko.txt".format(out_put)
    output_symbol_ko = "{}/abi_required_symbol_ko.txt".format(out_put)
    # extract the second column and remove duplicates
    get_symbols_and_ko_list(file_path, output_list, output_ko, output_symbol_ko)

    print("[6/15] get_approval_config_from_file...")
    input_file = "{}/abi_symbols_tmp_approval.csv".format(out_put)
    output_file = "{}/abi_symbols_tmp_approval.txt".format(out_put)
    get_approval_config_from_file(input_file, output_file)

    print("[7/15] compare_and_output_extra_lines...")
    base_file = "{}/abi_symbols_tmp_approval.txt".format(out_put)
    new_file = "{}/abi_required_symbol.txt".format(out_put)
    output_file = "{}/abi_required_and_remove_tmp_approval.txt".format(out_put)
    compare_and_output_extra_lines(base_file, new_file, output_file)

    try:
        #print("check abi_required_and_remove_tmp_approval.txt is empty or not...")
        check_file_is_empty(output_file)
    except Exception as e:
        print(e)
        copy_log_files_to_dest(out_put, log_dir, exclude_file)
        sys.exit(1)

    print("[8/15] compare recovery install ko missing sybmols...")
    base_file = "{}/modules.load.recovery".format(out_put)
    new_file = "{}/abi_required_ko.txt".format(out_put)
    output_file = "{}/abi_compare_recovery_ko_need_symbol.txt".format(out_put)
    #compare_same_line_files(base_file, new_file, output_file)

    #try:
    #    print("[9/15] check abi_compare_recovery_ko_need_symbol.txt is empty or not...")
    #    check_file_is_empty(output_file)
    #except Exception as e:
    #    print(e)
    #    copy_log_files_to_dest(out_put, log_dir, exclude_file)
    #    sys.exit(1)

    print("[10/15] generate all ko modversions...")
    dump_modversions(find_ko_files(out_put))

    input_file = "{}/ko_modversions_full.txt".format(out_put)
    output_file = "{}/ko_modversions_remove_duplicates_and_sort.txt".format(out_put)
    remove_duplicates_and_sort_by_second_column(input_file, output_file)

    input_file = "{}/ko_modversions_remove_duplicates_and_sort.txt".format(out_put)
    output_file = "{}/ko_modversions_last_remove_spaces.txt".format(out_put)
    tab_to_spaces(input_file, output_file)

    print("[11/15] get gki modversions from vmlinux_gki.symvers...")
    input_file = "{}/vmlinux_gki.symvers".format(out_put)
    output_file = "{}/vmlinux_gki_modversions_all_crc_symbols".format(out_put)
    generate_modversions_from_src_to_dst(input_file, output_file)

    input_file = "{}/vmlinux_gki_modversions_all_crc_symbols".format(out_put)
    output_file = "{}/vmlinux_gki_modversions_crc_symbols_remove_duplicates_and_sort".format(out_put)
    remove_duplicates_and_sort_by_second_column(input_file, output_file)

    print("[12/15] get oki modversions from vmlinux.symvers...")
    input_file = "{}/vmlinux.symvers".format(out_put)
    output_file = "{}/vmlinux_oki_modversions_all_crc_symbols".format(out_put)
    generate_modversions_from_src_to_dst(input_file, output_file)

    input_file = "{}/vmlinux_oki_modversions_all_crc_symbols".format(out_put)
    output_file = "{}/vmlinux_oki_modversions_crc_symbols_remove_duplicates_and_sort".format(out_put)
    remove_duplicates_and_sort_by_second_column(input_file, output_file)

    print("[13/15] check ko modversions base on gki...")
    current_file = "{}/ko_modversions_last_remove_spaces.txt".format(out_put)
    base_file = "{}/vmlinux_gki_modversions_crc_symbols_remove_duplicates_and_sort".format(out_put)
    output_file = "{}/check_gki_symvers_ko_result.txt".format(out_put)
    compare_symbols_modversions(current_file, base_file, output_file)

    try:
        check_file_is_empty(output_file)
    except Exception as e:
        print(e)
        copy_log_files_to_dest(out_put, log_dir, exclude_file)
        sys.exit(1)

    print("[14/15] check ko modversions base on oki...")
    current_file = "{}/ko_modversions_last_remove_spaces.txt".format(out_put)
    base_file = "{}/vmlinux_oki_modversions_crc_symbols_remove_duplicates_and_sort".format(out_put)
    output_file = "{}/check_oki_symvers_ko_result.txt".format(out_put)
    compare_symbols_modversions(current_file, base_file, output_file)

    try:
        check_file_is_empty(output_file)
    except Exception as e:
        print(e)
        copy_log_files_to_dest(out_put, log_dir, exclude_file)
        sys.exit(1)
    print("[15/15] copy log to dest...")
    copy_log_files_to_dest(out_put, log_dir, exclude_file)
    print("abi_gki_oki_check.py end !!!\n")

    end_time = datetime.now()
    print("End time:", end_time.strftime("%Y-%m-%d %H:%M:%S"))
    elapsed_time = end_time - start_time
    print("Total time:", format_duration(elapsed_time))

if __name__ == "__main__":
    main()
