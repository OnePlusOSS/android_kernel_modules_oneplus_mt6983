load("//build/kernel/kleaf:kernel.bzl", "ddk_headers")
load("//build/kernel/oplus:oplus_modules_define.bzl", "define_oplus_ddk_module")
load("//build/kernel/oplus:oplus_modules_dist.bzl", "ddk_copy_to_dist_dir")

def define_oplus_local_modules():

    define_oplus_ddk_module(
        name = "oplus_bsp_pogo_keyboard",
        srcs = native.glob([
            "**/*.h",
            "pogo_keyboard_core.c",
            "keyboard.c",
            "touchpad.c",
            "pogo_tty_io.c",
        ]),
        includes = ["."],
        conditional_defines = {
            "qcom":  ["CONFIG_QCOM_PANEL_EVENT_NOTIFIER"],
        },
    )

    ddk_copy_to_dist_dir(
        name = "oplus_bsp_pogo_keyboard",
        module_list = [
            "oplus_bsp_pogo_keyboard",
        ],
    )
