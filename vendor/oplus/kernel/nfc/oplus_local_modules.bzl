load("//build/kernel/kleaf:kernel.bzl", "ddk_headers")
load("//build/kernel/oplus:oplus_modules_define.bzl", "define_oplus_ddk_module")
load("//build/kernel/oplus:oplus_modules_dist.bzl", "ddk_copy_to_dist_dir")

def define_oplus_local_modules():

    define_oplus_ddk_module(
        name = "oplus_network_nfc_sn_ese",
        srcs = native.glob([
            "**/*.h",
            "sn_nci/ese/p73.c",
        ]),
        includes = ["."],
        ko_deps = [
            "//vendor/oplus/kernel/nfc:oplus_network_nfc_i2c",
        ],
    )

    define_oplus_ddk_module(
        name = "oplus_network_nfc_i2c",
        srcs = native.glob([
            "**/*.h",
            "sn_nci/nfc/common.c",
            "sn_nci/nfc/common_ese.c",
            "sn_nci/nfc/i2c_drv.c",
        ]),
        includes = ["."],
    )

    ddk_copy_to_dist_dir(
        name = "oplus_network_nfc",
        module_list = [
            "oplus_network_nfc_sn_ese",
            "oplus_network_nfc_i2c",
        ],
    )
