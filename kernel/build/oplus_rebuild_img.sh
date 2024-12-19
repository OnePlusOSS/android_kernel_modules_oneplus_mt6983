#!/bin/bash
function init_image_environment() {
    #source kernel/build/oplus_setup.sh $1
    #set -x
    TOPDIR=${PWD}
    ACKDIR=${TOPDIR}/kernel
    TOOLS=${ACKDIR}/oplus/tools
    ORIGIN_IMAGE=${ACKDIR}/oplus/prebuild/origin_img
    BOOT_TMP_IMAGE=${ACKDIR}/oplus/prebuild/boot_tmp
    IMAGE_OUT=${ACKDIR}/oplus/prebuild/out
    VENDOR_BOOT_TMP_IMAGE=${ACKDIR}/oplus/prebuild/vendor_boot_tmp
    VENDOR_DLKM_TMP_IMAGE=${ACKDIR}/oplus/prebuild/vendor_dlkm_tmp
    DT_TMP_IMAGE=${ACKDIR}/oplus/prebuild/dt_tmp
    DT_DIR=${ACKDIR}/out/
    PYTHON_TOOL="${ACKDIR}/prebuilts/build-tools/path/linux-x86/python3"
    MKBOOTIMG_PATH=${TOPDIR}/"system/tools/mkbootimg/mkbootimg.py"
    UNPACK_BOOTIMG_TOOL="${TOPDIR}/system/tools/mkbootimg/unpack_bootimg.py"
    LZ4="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/lz4"
    CPIO="${ACKDIR}/prebuilts/build-tools/path/linux-x86/cpio"
    SIMG2IMG="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/simg2img"
    IMG2SIMG="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/img2simg"
    EROFS="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/mkfs.erofs"
    BUILD_IMAGE="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/build_image"
    MKDTIMG="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/mkdtimg"
    MKBOOTFS="${ACKDIR}/prebuilts/kernel-build-tools/linux-x86/bin/mkbootfs"
    version="6.1"
    AVBTOOL=${TOPDIR}/"prebuilts/kernel-build-tools/linux-x86/bin/avbtool"
    #IMAGE_SERVER="http://gpw13.myoas.com/artifactory/phone-snapshot-local/PSW/MT6989_14/Nvwa/22113/Daily/PublicMarket/BringUp/domestic/userdebug/14.0.0.1_2023062004070410_userdebug"
    mkdir -p ${IMAGE_OUT}
}
function download_prebuild_image() {
    if [[ ! -e "${ORIGIN_IMAGE}/vendor_boot.img" ]]; then
        mkdir -p ${ORIGIN_IMAGE}

        if [ -z ${IMAGE_SERVER} ]; then
            echo ""
            echo ""
            echo "you need input base version like this:"
            echo "http://gpw13.myoas.com/artifactory/phone-snapshot-local/PSW/MT6989_14/Nvwa/22113/Daily/PublicMarket/BringUp/domestic/userdebug/14.0.0.1_2023062004070410_userdebug"
            echo ""
            echo "or you exit it and then exoprt like this:"
            echo "export IMAGE_SERVER=http://gpw13.myoas.com/artifactory/phone-snapshot-local/PSW/MT6989_14/Nvwa/22113/Daily/PublicMarket/BringUp/domestic/userdebug/14.0.0.1_2023062004070410_userdebug"
            read IMAGE_SERVER
        fi

        if ! wget -qS ${IMAGE_SERVER}/compile.ini; then
            echo "server can't connect,please set IMAGE_SERVER and try again"
            return
        fi

        wget ${IMAGE_SERVER}/compile.ini  -O ${ORIGIN_IMAGE}/compile.ini
        OFP_DRI=`cat ${ORIGIN_IMAGE}/compile.ini | grep "ofp_folder =" | awk '{print $3 }'`
        wget ${IMAGE_SERVER}/${OFP_DRI}/IMAGES/boot.img -O ${ORIGIN_IMAGE}/boot.img
        wget ${IMAGE_SERVER}/${OFP_DRI}/IMAGES/vendor_boot.img -O ${ORIGIN_IMAGE}/vendor_boot.img
        wget ${IMAGE_SERVER}/${OFP_DRI}/IMAGES/system_dlkm.img -O ${ORIGIN_IMAGE}/system_dlkm.img
        wget ${IMAGE_SERVER}/${OFP_DRI}/IMAGES/vendor_dlkm.img -O ${ORIGIN_IMAGE}/vendor_dlkm.img
        wget ${IMAGE_SERVER}/${OFP_DRI}/IMAGES/dtbo.img -O ${ORIGIN_IMAGE}/dtbo.img
        wget ${IMAGE_SERVER}/${OFP_DRI}/META/misc_info.txt -O ${ORIGIN_IMAGE}/misc_info.txt
        cp ${ACKDIR}/build/oem_prvk.pem ${ORIGIN_IMAGE}/
    fi
}

function sign_boot_image() {

    algorithm=$(awk -F '[= ]' '$1=="avb_boot_algorithm" {$1="";print}' ${ORIGIN_IMAGE}/misc_info.txt)
    partition_size=$(awk -F '[= ]' '$1=="boot_size" {$1="";print}' ${ORIGIN_IMAGE}/misc_info.txt)
    footer_args=$(awk -F '[= ]' '$1=="avb_boot_add_hash_footer_args" {$1="";print}' ${ORIGIN_IMAGE}/misc_info.txt)
    partition_name=boot
    salt=`uuidgen | sed 's/-//g'`
    ${AVBTOOL} add_hash_footer \
        --image ${IMAGE_OUT}/boot.img  \
        --partition_name ${partition_name} \
        --partition_size ${partition_size}\
        --algorithm ${algorithm} \
        --key ${ORIGIN_IMAGE}/oem_prvk.pem \
        --salt ${salt} \
        ${footer_args}
}

rebuild_boot_image() {
    echo "rebuild boot.img"
    rm -rf ${BOOT_TMP_IMAGE}/*
    boot_mkargs=$(${PYTHON_TOOL} ${UNPACK_BOOTIMG_TOOL} --boot_img ${ORIGIN_IMAGE}/boot.img --out ${BOOT_TMP_IMAGE} --format=mkbootimg)
    cp ${TOPDIR}/out_krn/target/product/mgk_64_k510/obj/KERNEL_OBJ/kernel-5.10/arch/arm64/boot/Image.gz ${BOOT_TMP_IMAGE}/kernel
    bash -c "${PYTHON_TOOL} ${MKBOOTIMG_PATH} ${boot_mkargs} -o ${IMAGE_OUT}/boot.img"
    sign_boot_image
}

rebuild_vendor_boot_image() {
    echo "rebuild vendor_boot.img"
    rm -rf ${VENDOR_BOOT_TMP_IMAGE}/*
    boot_mkargs=$(${PYTHON_TOOL} ${UNPACK_BOOTIMG_TOOL} --boot_img ${ORIGIN_IMAGE}/vendor_boot.img --out ${VENDOR_BOOT_TMP_IMAGE}/origin --format=mkbootimg)
    #index="00 01"
    index="00"
    for index in  $index
    do
        echo " index  $index "
        ls -l ${VENDOR_BOOT_TMP_IMAGE}
        mv ${VENDOR_BOOT_TMP_IMAGE}/origin/vendor_ramdisk${index} ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}.lz4
        ls -l ${VENDOR_BOOT_TMP_IMAGE}
        ${LZ4} -d ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}.lz4
        rm ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}.lz4
        mkdir -p ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}
        mv ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index} ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}/vendor_ramdisk${index}
        pushd  ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}
        ${CPIO} -idu < ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}/vendor_ramdisk${index}
        popd
        cp kernel/bazel-bin/kernel_device_modules-${version}/mgk_customer_modules_install.${variants_type}/oplus_bsp_boot_projectinfo.ko \
        ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}/lib/modules/
        rm ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index}/vendor_ramdisk${index}

        ${MKBOOTFS} -d ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index} ${VENDOR_BOOT_TMP_IMAGE}/ramdisk${index} > ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}
        ${LZ4} -l -12 --favor-decSpeed ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}
        ls -l ${VENDOR_BOOT_TMP_IMAGE}
        mv ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}.lz4 ${VENDOR_BOOT_TMP_IMAGE}/origin/vendor_ramdisk${index}
        rm ${VENDOR_BOOT_TMP_IMAGE}/vendor_ramdisk${index}
        ls -l ${VENDOR_BOOT_TMP_IMAGE}
        #cp ${DT_TMP_IMAGE}/dtb.img ${VENDOR_BOOT_TMP_IMAGE}/origin/dtb
    done
    bash -c "${PYTHON_TOOL} ${MKBOOTIMG_PATH} ${boot_mkargs} --vendor_boot ${IMAGE_OUT}/vendor_boot.img"

}

rebuild_vendor_dlkm_image() {
    echo "rebuild vendor_dlkm.img"
    mkdir -p ${VENDOR_DLKM_TMP_IMAGE}
    ${SIMG2IMG} ${ORIGIN_IMAGE}/vendor_dlkm.img  ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm.img
    ${TOOLS}/7z_new ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm.img ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm_out
    ${BUILD_IMAGE} ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm_out ${TOOLS}/vendor_dlkm_image_info.txt ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm.img /dev/null
}

rebuild_vendor_dlkm_erofs_image() {
    echo "rebuild vendor_dlkm.img"
    rm -rf ${VENDOR_DLKM_TMP_IMAGE}/*
    mkdir -p ${VENDOR_DLKM_TMP_IMAGE}
    ${SIMG2IMG} ${ORIGIN_IMAGE}/vendor_dlkm.img  ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm.img
    ${TOOLS}/erofs_unpack.sh ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm.img  ${VENDOR_DLKM_TMP_IMAGE}/mnt ${VENDOR_DLKM_TMP_IMAGE}/out
    touch ${VENDOR_DLKM_TMP_IMAGE}/out/lib/modules/readme.txt
    ${EROFS} ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm_repack.img  ${VENDOR_DLKM_TMP_IMAGE}/out

    ${IMG2SIMG} ${VENDOR_DLKM_TMP_IMAGE}/vendor_dlkm_repack.img  ${IMAGE_OUT}/vendor_dlkm.img
}
rebuild_system_dlkm_erofs_image() {
    echo "rebuild system_dlkm.img"
    rm -rf ${SYSTEM_DLKM_TMP_IMAGE}/*
    mkdir -p ${SYSTEM_DLKM_TMP_IMAGE}
    ${SIMG2IMG} ${ORIGIN_IMAGE}/system_dlkm.img  ${SYSTEM_DLKM_TMP_IMAGE}/system_dlkm.img
    ${TOOLS}/erofs_unpack.sh ${SYSTEM_DLKM_TMP_IMAGE}/system_dlkm.img  ${SYSTEM_DLKM_TMP_IMAGE}/mnt ${SYSTEM_DLKM_TMP_IMAGE}/out
    #rm -rf ${SYSTEM_DLKM_TMP_IMAGE}/out/lib/modules/6.1.23-android14-4-g5cf7530f7c55-qki-consolidate/kernel/*
    #cp -r device/qcom/pineapple-kernel/system_dlkm/lib/modules/6.1.23-android14-4-00873-g5cf7530f7c55-qki-consolidate/kernel/* \
    #${SYSTEM_DLKM_TMP_IMAGE}/out/lib/modules/6.1.23-android14-4-g5cf7530f7c55-qki-consolidate/kernel/
    touch ${SYSTEM_DLKM_TMP_IMAGE}/out/lib/modules/readme.txt
    ${EROFS} ${SYSTEM_DLKM_TMP_IMAGE}/system_dlkm_repack.img  ${SYSTEM_DLKM_TMP_IMAGE}/out

    ${IMG2SIMG} ${SYSTEM_DLKM_TMP_IMAGE}/system_dlkm_repack.img  ${IMAGE_OUT}/system_dlkm.img
}

rebuild_dtb_image() {
    echo "rebuild dtb.img"
    mkdir -p ${DT_TMP_IMAGE}
    cp ${DT_DIR}/*.dtb ${DT_TMP_IMAGE}
    ${MKDTIMG} create  ${DT_TMP_IMAGE}/dtb.img  ${DT_TMP_IMAGE}/*.dtb
}

rebuild_dtbo_image() {
    echo "rebuild dtbo.img"
    mkdir -p ${DT_TMP_IMAGE}
    cp ${DT_DIR}/*.dtbo ${DT_TMP_IMAGE}
    ${MKDTIMG} create ${DT_TMP_IMAGE}/dtbo.img  ${DT_TMP_IMAGE}/*.dtbo
}

init_image_environment $1
download_prebuild_image
rebuild_boot_image
#rebuild_system_dlkm_erofs_image
#rebuild_vendor_dlkm_erofs_image
#rebuild_dtb_image
#rebuild_dtbo_image
rebuild_vendor_boot_image
