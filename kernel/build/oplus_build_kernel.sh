#!/bin/bash

source kernel/build/oplus_setup.sh $1
init_build_environment


function build_kernel_cmd() {
    mkdir -p ${TOPDIR}/LOGDIR
    cd ${TOPDIR}/kernel
    python kernel-5.10/scripts/gen_build_config.py --kernel-defconfig mgk_64_k510_defconfig --kernel-defconfig-overlays oplus${variants_platform}.config --kernel-build-config-overlays "" -m user -o ../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ/build.config |tee ${TOPDIR}/LOGDIR/${current_log}.log
	ENABLE_GKI_CHECKER= CC_WRAPPER= SKIP_MRPROPER=1 BUILD_CONFIG=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ/build.config OUT_DIR=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ DIST_DIR=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ POST_DEFCONFIG_CMDS="exit 0" ./build/build.sh |tee ${TOPDIR}/LOGDIR/${current_log}.log
	CHIPSET_COMPANY=MTK OPLUS_VND_BUILD_PLATFORM=${OPLUS_VND_BUILD_PLATFORM} CC_WRAPPER= SKIP_MRPROPER=1 BUILD_CONFIG=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ/build.config OUT_DIR=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ DIST_DIR=../out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ SKIP_DEFCONFIG=1 ./build/build_abi.sh |tee ${TOPDIR}/LOGDIR/${current_log}.log
    cd ${TOPDIR}/
}


function mk_dtboimg_cfg() {

    echo "file $1  out $2"

    echo $1.dtb >>$2
    dts_file=$1.dts
    dtsino=`grep -m 1 'oplus_boardid,dtsino' $dts_file | sed 's/ //g' | sed 's/.*oplus_boardid\,dtsino\=\"//g' | sed 's/\"\;.*//g'`
    pcbmask=`grep -m 1 'oplus_boardid,pcbmask' $dts_file | sed 's/ //g' | sed 's/.*oplus_boardid\,pcbmask\=\"//g' | sed 's/\"\;.*//g'`

    echo " id=$my_dtbo_id" >>$2
    echo " custom0=$dtsino" >> $2
    echo " custom1=$pcbmask" >> $2
    let my_dtbo_id++
}

function build_dt_cmd() {
    mkdir -p ${TOPDIR}/LOGDIR

    rm ${DEVICE_TREE_OUT}/dtboimg.cfg
    mkdir -p ${KERNEL_OBJ}
    cp ${TOPDIR}/kernel/build/_setup_env.sh ${KERNEL_OBJ}/mtk_setup_env.sh

    cd ${TOPDIR}/kernel
    python kernel-${VERSION}/scripts/gen_build_config.py \
          --kernel-defconfig gki_defconfig --kernel-defconfig-overlays "" \
          --kernel-build-config-overlays "" \
          -m user -o ${KERNEL_OBJ}/build.config

    for dtb in  $dtb_support_list
    do
        echo " dtb  $dtb "
        mkdir -p ${DEVICE_TREE_OUT}/mediatek
        cp ${DEVICE_TREE_SRC}/${dtb}.dts ${DEVICE_TREE_OUT}/mediatek/${dtb}.dts
        cp ${TOPDIR}/out_krn/target/product/${KRN_MGK}/obj/KERNEL_OBJ/kernel-${VERSION}/include ${KERNEL_OBJ}/kernel-${VERSION} -rf
        cp ${TOPDIR}/out_krn/target/product/${KRN_MGK}/obj/KERNEL_OBJ/kernel-${VERSION}/scripts ${KERNEL_OBJ}/kernel-${VERSION} -rf
        cp ${TOPDIR}/out_krn/target/product/${KRN_MGK}/obj/KERNEL_OBJ/kernel-${VERSION}/source ${KERNEL_OBJ}/kernel-${VERSION} -rf
        cp ${TOPDIR}/out_krn/target/product/${KRN_MGK}/obj/KERNEL_OBJ/kernel-${VERSION}/Makefile ${KERNEL_OBJ}/kernel-${VERSION}

        OUT_DIR=../${RELATIVE_KERNEL_OBJ} BUILD_CONFIG=../${RELATIVE_KERNEL_OBJ}/build.config GOALS=dtbs ../${BUILD_TOOLS}/build_kernel.sh mediatek/${dtb}.dtb
        cat ${DEVICE_TREE_OUT}/mediatek/${dtb}.dtb > ${DEVICE_TREE_OUT}/mediatek/dtb
    done

    for dtbo in  $dtbo_support_list
    do
        echo " dtbo  $dtbo "
        mkdir -p ${DEVICE_TREE_OUT}/${dtbo}
        cp ${DEVICE_TREE_SRC}/${dtbo}.dts ${DEVICE_TREE_OUT}/mediatek/${dtbo}.dts
        mk_dtboimg_cfg ${DEVICE_TREE_OUT}/mediatek/${dtbo} ${DEVICE_TREE_OUT}/dtboimg.cfg
        ${DRV_GEN}  ${DWS_SRC}/${dtbo}.dws ${DEVICE_TREE_OUT}/${dtbo} ${DEVICE_TREE_OUT}/${dtbo} cust_dtsi
        #we must use Relative path,then we build success,or will build fail
        OUT_DIR=../${RELATIVE_KERNEL_OBJ} BUILD_CONFIG=../${RELATIVE_KERNEL_OBJ}/build.config GOALS=dtbs ../${BUILD_TOOLS}/build_kernel.sh mediatek/${dtbo}.dtb
    done

    cd ${TOPDIR}
    ${MKDTIMG} cfg_create ${DEVICE_TREE_OUT}/mediatek/dtbo.img  ${DEVICE_TREE_OUT}/dtboimg.cfg
    ./kernel/build/osign_client --platform=mt${variants_platform} --android_version=s --input_path=${DEVICE_TREE_OUT}/mediatek --input_name=dtbo.img --output_path=${DEVICE_TREE_OUT}/mediatek/oplus_signed/
    cp ${DEVICE_TREE_OUT}/mediatek/dtb ${IMAGE_OUT}/
    cp ${DEVICE_TREE_OUT}/mediatek/oplus_signed/dtbo.img ${IMAGE_OUT}/
 }
build_kernel_cmd
build_dt_cmd

