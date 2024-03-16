#!/bin/bash
#
# This script can compile the firmware for PanelDue and creates
# versions with a customized splash screen.
#
# Change log:
#
# 16 Mar 2024, wschadow, initial version
#

export ROOT=`pwd`
export NCORES=`nproc`

OS_FOUND=$( command -v uname)

case $( "${OS_FOUND}" | tr '[:upper:]' '[:lower:]') in
  linux*)
    TARGET_OS="linux"
   ;;
  msys*|cygwin*|mingw*)
    # or possible 'bash on windows'
    TARGET_OS='windows'
   ;;
  nt|win*)
    TARGET_OS='windows'
    ;;
  darwin)
    TARGET_OS='macos'
    ;;
  *)
    TARGET_OS='unknown'
    ;;
esac


# check operating system
echo
if [ $TARGET_OS == "linux" ]; then
    if [ $(uname -m) == "x86_64" ]; then
        echo -e "$(tput setaf 2)Linux 64-bit found$(tput sgr0)"
        Processor="64"
    elif [[ $(uname -m) == "i386" || $(uname -m) == "i686" ]]; then
        echo "$(tput setaf 2)Linux 32-bit found$(tput sgr0)"
        Processor="32"
    else
        echo "$(tput setaf 1)Unsupported OS: Linux $(uname -m)\n"
        exit -1
    fi
else
    echo -e "$(tput setaf 1)This script doesn't support your Operating system!"
    echo -e "Please use Linux 64-bit or Windows 10 64-bit with Linux subsystem / git-bash.$(tput sgr0)\n"
    exit -1
fi

# Check if CMake is installed
export CMAKE_INSTALLED=`which cmake`
if [[ -z "$CMAKE_INSTALLED" ]]
then
    echo "Can't find CMake. Either is not installed or not in the PATH. Aborting!"
    exit -1
fi

unset name
while getopts ":hpcaw" opt; do
  case ${opt} in

    p )
        BUILD_PANELDUE="1"
        ;;
    c )
        BUILD_CARIBOUVERSION="1"
        ;;
    a )
        BUILD_AMEXTRUSIONVERSION="1"
        ;;
    w )
	    BUILD_WIPE="1"
	    ;;
    h ) echo -e "\nUsage: ./BuildLinux.sh [-h][-w][-p][-c][-a]"
        echo "   -h: this message"
        echo "   -w: build Wipe firmware"
	    echo "   -p: build PanelDue firmware"
        echo "   -c: build Caribou version (inlcude Caribou splash screen)"
        echo -e "   -a: build AM Extrusion version (inlcude AM Extrusion splash screen)\n"
        exit 0
        ;;
  esac
done

if [ $OPTIND -eq 1 ]
then
    echo -e "\nUsage: ./BuildLinux.sh [-h][-w][-p][-c][-a]"
    echo "   -h: this message"
    echo "   -w: build Wipe firmware"
    echo "   -p: build PanelDue firmware"
    echo "   -c: build Caribou version (inlcude Caribou splash screen)"
    echo -e "   -a: build AM Extrusion version (inlcude AM Extrusion splash screen)\n"
    exit 0
fi

if [[ -n $BUILD_WIPE ]]
then
    echo -e "[0/4] Wiping build directory ...\n"
    rm -fr build
    echo -e " ... done"
fi

if [[ -n "$BUILD_PANELDUE" ]]
then
    if [[ -n $BUILD_WIPE ]]
    then
        echo -e "\n"
    fi
    echo -e "[1/4] Configuring PanelDue firmware ...\n"
    # mkdir build
    if [ ! -d "build" ]
    then
    	mkdir build
    fi

    # cmake
    pushd build > /dev/null

    cmake .. -DDEVICE=7.0i
    echo " ... done"
    # make PanelDue firmware
    echo -e "\n[2/4] Building PanelDue firmware ...\n"
    make -j$NCORES
    echo -e "\n ... done"
    popd > /dev/null
fi

if [[ -n "$BUILD_CARIBOUVERSION" ]]
then
    echo -e "\n[3/4] Building Caribou version ...\n"
    pushd build > /dev/null
    if [ ! -d "Caribou" ]
    then
    	mkdir Caribou
    fi
    # create bin file from bmp
    ../Tools/gobmp2c/linux/bmp2c -binary -outfile Caribou.bin ../SplashScreens/Caribou3dLogoScreen.bmp
    cat paneldue.bin Caribou.bin > Caribou/PanelDueFirmware.bin
    rm -f Caribou.bin
    popd > /dev/null
    echo -e " ... done"
fi

if [[ -n "$BUILD_AMEXTRUSIONVERSION" ]]
then
    echo -e "\n[4/4] Building AM Extrusion version ...\n"
    pushd build > /dev/null
    if [ ! -d "AMExtrusion" ]
    then
    	mkdir AMExtrusion
    fi
   ../Tools/gobmp2c/linux/bmp2c -binary -outfile AMExtrusion.bin ../SplashScreens/AMExtrusion-black.bmp
    cat paneldue.bin AMExtrusion.bin > AMExtrusion/PanelDueFirmware.bin
    rm -f AMExtrusion.bin
    popd > /dev/null
    echo -e " ... done"
fi

