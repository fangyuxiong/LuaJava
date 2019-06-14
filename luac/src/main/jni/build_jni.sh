#!/bin/bash
# 生成动态链接库so文件

function inform() {
    echo "usage: ./build_jni.sh <option>"
    echo "options:"
    echo "  -R: Release mode(without debug info)"
    echo "  -c: clean obj directory"
    echo "  -r: rebuild, clean first"
    echo "  -m: Log memory info"
    echo "  -t: support multi thread, default not support"
    echo "  -h: help"
}

function closeInfo() {
    sed -i '' "s/#*\(.*DJ_API_INFO\)/#\1/g" japi/Android.mk
}

function openInfo() {
    sed -i '' "s/#*\(.*DJ_API_INFO\)/\1/g" japi/Android.mk
}

function closeSaveLV() {
    sed -i '' "s/#*\(.*DSAVE_LUAVALUE\)/#\1/g" japi/Android.mk
}

function openSaveLV() {
    sed -i '' "s/#*\(.*DSAVE_LUAVALUE\)/\1/g" japi/Android.mk
}

function closeMemInfo() {
    sed -i '' "s/#*\(.*DMEM_INFO\)/#\1/g" japi/Android.mk
}

function openMemInfo() {
    sed -i '' "s/#*\(.*DMEM_INFO\)/\1/g" japi/Android.mk
}

function openMultiThread() {
    sed -i '' "s/#*\(.*DMULTI_THREAD\)/\1/g" lua/Android.mk
}

function closeMultiThread() {
    sed -i '' "s/#*\(.*DMULTI_THREAD\)/#\1/g" lua/Android.mk
}

R=0
c=0
r=0
s=0
m=0
t=0

while getopts "hRcrsm" optname
do
    case "$optname" in
        "h")
            inform
            exit
            ;;
        "R")
            R=1
            ;;
        "c")
            c=1
            ;;
        "r")
            r=1
            ;;
        "s")
            s=1
            ;;
        "m")
            m=1
            ;;
        "t")
            t=1
            ;;
        "?")
            echo "Unknown option $OPTARG"
            ;;
        ":")
            echo "No argument value for option $OPTARG"
            ;;
        *)
            inform
            exit
            ;;
    esac
done

if [[ ${t} -eq 1 ]]; then
    echo "=================build with multi thread supporting"
    openMultiThread
else
    echo "=================build without multi thread supporting"
    closeMultiThread
fi

if [[ ${m} -eq 1 ]]; then
    echo "=================build with memory info"
    openMemInfo
else
    echo "=================build without memory info"
    closeMemInfo
fi

if [[ ${R} -eq 1 ]]; then
    echo "=================build without debug info"
    closeInfo
else
    echo "=================build with debug info"
    openInfo
fi

if [[ ${s} -eq 1 ]]; then
    echo "=================build with save luavalue"
    openSaveLV
else
    echo "=================build without save luavalue"
    closeSaveLV
fi

if [[ ${r} -eq 1 ]]; then
    echo "=================rebuild, clean first"
    ndk-build clean
fi

ndk-build

if [[ ${c} -eq 1 ]]; then
    echo "=================clean obj directory after build"
    rm -rf ../obj
fi

openInfo
closeSaveLV
closeMemInfo
closeMultiThread

function rename_v7a() {
    mv ../libs/armeabi-v7a/libluajapi.so ../libs/armeabi/libluajapi.so
    cp ../obj/local/armeabi-v7a/libluajapi.so ../obj/local/armeabi/libluajapi.so
}

# 打so改为打v7架构so，兼容老版本，将名称重命名
rename_v7a