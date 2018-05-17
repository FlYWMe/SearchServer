#!/bin/bash



set -e


main()
{
    cd ../deps
    tar -zxf libopenblas.tar.gz && \
    sudo dpkg -i libblas-common_3.6.0-2ubuntu2_amd64.deb && \
    sudo dpkg -i libopenblas-base_0.2.18-1ubuntu1_amd64.deb && \
    sudo dpkg -i libopenblas-dev_0.2.18-1ubuntu1_amd64.deb
   echo "INFO - install openblas dependence finished!" 
}
main $# $@
