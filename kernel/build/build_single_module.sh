export BUILD_CONFIG=${OUT_DIR}/build.config
export DIST_DIR=${OUT_DIR}
export OUT_DIR=${OUT_DIR}
export ENABLE_GKI_CHECKER=
export CC_WRAPPER=/usr/bin/ccache SKIP_MRPROPER=1
export SKIP_DEFCONFIG=1
export SINGLE_MODULE_BUILD=1
export POST_KERNEL_MOD_BUILD_CMDS="exit 0"

./build/build.sh $@
