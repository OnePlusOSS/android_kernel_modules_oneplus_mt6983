load("//build/kernel/kleaf:kernel.bzl", "ddk_headers")
load("//build/kernel/oplus:oplus_modules_define.bzl", "define_oplus_ddk_module")
load("//build/kernel/oplus:oplus_modules_dist.bzl", "ddk_copy_to_dist_dir")


def define_oplus_local_modules():

    define_oplus_ddk_module(
        name = "iolimit",
        srcs = native.glob([
            "*.c",
            "*.h",
        ]),
        includes = ["."],
        local_defines = ["CONFIG_UNION_IOLIMIT"],
    )

    ddk_headers(
        name = "config_headers",
        includes = ["."],
    )

    ddk_copy_to_dist_dir(
        name = "iolimit",
        module_list = [
            "iolimit",
        ],
    )