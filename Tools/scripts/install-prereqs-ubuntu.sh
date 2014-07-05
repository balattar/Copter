#!/bin/bash
set -e

BASE_PKGS="gawk make git arduino-core curl"
SITL_PKGS="g++ python-pip python-matplotlib python-serial python-wxgtk2.8 python-scipy python-opencv python-numpy python-pyparsing ccache"
PYTHON_PKGS="pymavlink MAVProxy droneapi"
PX4_PKGS="python-serial python-argparse openocd flex bison libncurses5-dev \
          autoconf texinfo build-essential libftdi-dev libtool zlib1g-dev \
          zip genromfs"
UBUNTU64_PKGS="libc6:i386 libgcc1:i386 gcc-4.6-base:i386 libstdc++5:i386 libstdc++6:i386"
ASSUME_YES=false

# GNU Tools for ARM Embedded Processors
# (see https://launchpad.net/gcc-arm-embedded/)
ARM_ROOT="gcc-arm-none-eabi-4_8-2013q4"
ARM_TARBALL="$ARM_ROOT-20131204-linux.tar.bz2"
ARM_TARBALL_URL="https://launchpad.net/gcc-arm-embedded/4.8/4.8-2013-q4-major/+download/$ARM_TARBALL"

# Ardupilot Tools
ARDUPILOT_TOOLS="ardupilot/Tools/autotest"


function maybe_prompt_user() {
    if $ASSUME_YES; then
        return 0
    else
        read -p "$1"
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            return 0
        else
            return 1
        fi
    fi
}


OPTIND=1  # Reset in case getopts has been used previously in the shell.
while getopts "y" opt; do
    case "$opt" in
        \?)
            exit 1
            ;;
        y)  ASSUME_YES=true
            ;;
    esac
done

if $ASSUME_YES; then
    APT_GET="sudo apt-get -qq --assume-yes"
else
    APT_GET="sudo apt-get"
fi

sudo usermod -a -G dialout $USER

$APT_GET remove modemmanager
$APT_GET update
$APT_GET install $BASE_PKGS $SITL_PKGS $PX4_PKGS $UBUNTU64_PKGS
sudo pip -q install $PYTHON_PKGS


if [ ! -d ~/PX4Firmware ]; then
    git clone https://github.com/diydrones/PX4Firmware.git ~/PX4Firmware
fi

if [ ! -d ~/PX4NuttX ]; then
    git clone https://github.com/diydrones/PX4NuttX.git ~/PX4NuttX
fi

if [ ! -d ~/$ARM_ROOT ]; then
    (
        cd ~;
        curl -OL $ARM_TARBALL_URL;
        tar xjf ${ARM_TARBALL};
        rm ${ARM_TARBALL};
    )
fi

exportline="export PATH=$HOME/$ARM_ROOT/bin:\$PATH";
if ! grep -Fxq "$exportline" ~/.profile ; then
    if maybe_prompt_user "Add $HOME/$ARM_ROOT/bin to your PATH [Y/n]?" ; then
        echo $exportline >> ~/.profile
        $exportline
    else
        echo "Skipping adding $HOME/$ARM_ROOT/bin to PATH."
    fi
fi

exportline2="export PATH=$HOME/$ARDUPILOT_TOOLS:\$PATH";
if ! grep -Fxq "$exportline2" ~/.profile ; then
    if maybe_prompt_user "Add $HOME/$ARDUPILOT_TOOLS to your PATH [Y/n]?" ; then
        echo $exportline2 >> ~/.profile
        $exportline2
    else
        echo "Skipping adding $HOME/$ARDUPILOT_TOOLS to PATH."
    fi
fi

