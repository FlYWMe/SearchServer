#!/bin/bash


#------------------------------------------------------------------------------
#
# - git clone --recursive
# - ./install_deps.sh
# - cmake && make
#

set -e

# Check for 'uname' and abort if it is not available.
uname -v > /dev/null 2>&1 || { echo >&2 "ERROR - verifier requires 'uname' to identify the platform."; exit 1; }
# 是否有GPU
lspci|grep -i NVIDIA > /dev/null 2>&1 || { echo >&2 "ERROR - cannot find GPU !"; exit 1; }

case $(uname -s) in

#------------------------------------------------------------------------------
# Linux
#------------------------------------------------------------------------------
Linux)

    # Detect if sudo is needed.
    if [ $(id -u) != 0 ]; then
        SUDO="sudo"
    fi

#------------------------------------------------------------------------------
# Arch Linux
#------------------------------------------------------------------------------

    if [ -f "/etc/arch-release" ]; then

        echo "Installing CUDA and CuDNN on Arch Linux."

        # The majority of our dependencies can be found in the
        # Arch Linux official repositories.
        # See https://wiki.archlinux.org/index.php/Official_repositories
        
        #TODO

    elif [ -f "/etc/os-release" ]; then

        DISTRO_NAME=$(. /etc/os-release; echo $NAME)
        case $DISTRO_NAME in

        Debian*)
            echo "Installing CUDA and CuDNN on Debian Linux."
            # TODO
            ;;

        Fedora)
            echo "Installing CUDA and CuDNN on Fedora Linux."
            # TODO
            ;;

#------------------------------------------------------------------------------
# Ubuntu
#
# TODO - I wonder whether all of the Ubuntu-variants need some special
# treatment?
#
# TODO - We should also test this code on Ubuntu Server, Ubuntu Snappy Core
# and Ubuntu Phone.
#
# TODO - Our Ubuntu build is only working for amd64 and i386 processors.
# It would be good to add armel, armhf and arm64.
# See https://github.com/ethereum/webthree-umbrella/issues/228.
#------------------------------------------------------------------------------
        Ubuntu|LinuxMint)

            # if [[ -f "/usr/local/cuda-8.0" ]]; then
            #     echo "INFO - cuda already installed!";
            #     exit 1;
            # fi

            # echo "INFO - Installing CUDA and cudnn on Ubuntu."
            echo "INFO - Installing CUDA on Ubuntu."
            # install nvidia driver and cuda
            UBUNTU_VERSION=$(. /etc/os-release; echo $VERSION_ID)
            BIT="x86_64"
            DOWNLOAD_PREFIX="http://developer.download.nvidia.com/compute/cuda/repos/"
            # echo $UBUNTU_VERSION;

            case $UBUNTU_VERSION in 

            14.04)
                DOWNLOAD_LINK="ubuntu1404/${BIT}/cuda-repo-ubuntu1404_8.0.61-1_amd64.deb"
                ;;
        
            16.04)
                DOWNLOAD_LINK="ubuntu1604/${BIT}/cuda-repo-ubuntu1604_8.0.44-1_amd64.deb"
                ;;
            *)
                echo "ERROR - Cannot find support for current Ubuntu version"
                exit 1
                ;;
            esac
                
            DOWNLOAD_LINK=$DOWNLOAD_PREFIX$DOWNLOAD_LINK
            
            if [ -z "$(dpkg -l | grep cuda-repo)" ]; then
                wget -nv $DOWNLOAD_LINK
                $SUDO dpkg -i ${DOWNLOAD_LINK##*/}
            fi

            if [ -z "$(dpkg -l | grep cuda-drivers)" ]; then 
                $SUDO apt-get update
                $SUDO apt-get install -y cuda
            fi
            
            if [ -e "${DOWNLOAD_LINK##*/}" ]; then
                $SUDO rm ${DOWNLOAD_LINK##*/}
            fi

            echo "INFO - Install CUDA finished."

            # CUDNN_PACKAGE="cudnn-8.0-linux-x64-v5.1.tar"
            # CUDNN_PATH="./deps"
            # # install cudnn
            # if [ ! -e "${CUDNN_PATH}/${CUDNN_PACKAGE}" ];
            # then 
            #     echo "ERROR - can not find cudnn tar file ${CUDNN_PATH}/${CUDNN_PACKAGE}!";
            #     exit 1;
            # fi
            # cd $CUDNN_PATH
            # tar -xf $CUDNN_PACKAGE
            # $SUDO cp cuda/include/* /usr/local/cuda/include/
            # $SUDO cp cuda/lib64/* /usr/local/cuda/lib64/
            # rm -rf ./cuda
            # if [ -z "$(cat /etc/profile | grep /usr/local/cuda/lib64)" ]; then
            #     echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/usr/local/cuda/lib64" | $SUDO tee -a /etc/profile
            # fi
            # echo "INFO - Install cudnn finished."

            ;;

        CentOS*)
            echo "Installing CUDA and CuDNN on CentOS."
            # Enable EPEL repo that contains leveldb-devel
                        
            # TODO
            
            ;;

        *)
            echo "Unsupported Linux distribution: $DISTRO_NAME."
            exit 1
            ;;

        esac

    elif [ -f "/etc/alpine-release" ]; then
        # Alpine Linux
        echo "Installing CUDA and CuDNN on Alpine Linux."
        #TODO
        
    else

        case $(lsb_release -is) in

#------------------------------------------------------------------------------
# Other (unknown) Linux
# Major and medium distros which we are missing would include Mint, CentOS,
# RHEL, Raspbian, Cygwin, OpenWrt, gNewSense, Trisquel and SteamOS.
#------------------------------------------------------------------------------
        *)
            #other Linux
            echo "ERROR - Unsupported or unidentified Linux distro."
            exit 1
            ;;
        esac
    fi
    ;;

#------------------------------------------------------------------------------
# Other platform (not Linux, FreeBSD or macOS).
# Not sure what might end up here?
# Maybe OpenBSD, NetBSD, AIX, Solaris, HP-UX?
#------------------------------------------------------------------------------
*)
    #other
    echo "ERROR - Unsupported or unidentified operating system."
    ;;
esac
