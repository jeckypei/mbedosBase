

TARGET=WL1_F407VG
MBED_OS_VER=5.6.2

## Warn: change this dir to your own dir, must do it 
GCC_DIR="/Home/Downloads/gcc-arm-none-eabi-6-2017-q2-update/bin/"
## Warn: change this dir to your own dir, must do it 
RELEASE_DIR=/Volumes/C/version/

svn up

export GCC_ARM_PATH=${GCC_DIR}

MBED_OS_FILE="package/mbed-os-${MBED_OS_VER}.tar.gz"
IMAGE=BUILD/${TARGET}/GCC_ARM/mbed-os.bin

if [ ! -e mbed-os ]; then
    tar xvf ${MBED_OS_FILE}   
    mv  mbed-os-mbed-os-${MBED_OS_VER}  mbed-os
fi

if [ ! -e mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/TARGET_${TARGET} ];then
    svn update board
    mv   board/${MBED_OS_VER}/TARGET_${TARGET}  mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/
    patch -f  mbed-os/targets/targets.json < board/${MBED_OS_VER}/targets.json.patch
fi

rm -rf board/*

if [ ! -e .mbed ]; then
mbed new . 
mbed target ${TARGET} 
mbed toolchain GCC_ARM 
mbed config -G GCC_ARM_PATH="${GCC_DIR}"
fi

mbed compile
ls -l ${IMAGE} 
cp ${IMAGE}  ${RELEASE_DIR} 
ls -l  ${RELEASE_DIR} 

