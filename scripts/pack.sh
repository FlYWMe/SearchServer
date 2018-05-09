#!/bin/bash

set -e

PROJECT_DIR=$(pwd)
RELEASE_DIR="${PROJECT_DIR}/release"
EXECUTABLE_DIR="${PROJECT_DIR}/build/bin"
EXECUTABLE_FILE="${EXECUTABLE_DIR}/queryServer"
TARGET_DIR="${RELEASE_DIR}/queryServer"
SCRIPT_PATH="${PROJECT_DIR}/scripts"
FAISS_FILE="${PROJECT_DIR}/deps/faiss-1.0.tar.gz"
BLAS_FILE="${PROJECT_DIR}/deps/libopenblas.tar.gz"
BUILD_TYPE=0 # 0 is debug, 1 is release

pre_check()
{
	if [ -e ${RELEASE_DIR} ]; then
		rm -rf ${RELEASE_DIR}
	fi

	if [ ! -d ${TARGET_DIR} ]; then
		mkdir -p ${TARGET_DIR}
	fi

	cd ${TARGET_DIR}

	if [ -e "queryServer.tar.gz" ]; then
		rm queryServer.tar.gz
	fi 

	if [ ! -e $EXECUTABLE_FILE ]; then
		echo "ERROR - can not find executable file ${EXECUTABLE_FILE}!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install_deps.sh" ]; then
		echo "ERROR - can not find install dependence scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install_cuda.sh" ]; then
		echo "ERROR - can not find install cuda scripts!";
		exit 1;
	fi

	if [ ! -e "${SCRIPT_PATH}/install.sh" ]; then
		echo "ERROR - can not find install scripts!";
		exit 1;
	fi

	if [ ! -e "${FAISS_FILE}" ]; then
		echo "ERROR - can not find faiss file ${FAISS_FILE}!";
		exit 1;
	fi
	if [ ! -e "${BLAS_FILE}" ]; then
		echo "ERROR - can not find openblas file ${BLAS_FILE}!";
		exit 1;
	fi
	echo "Check files done!"
}

make_dir()
{
	mkdir $TARGET_DIR/scripts
	echo "Gennerate directories done!"
}
 

copy_files()
{
	cp $EXECUTABLE_FILE $TARGET_DIR
	cp ${SCRIPT_PATH}/install_deps.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/install_cuda.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/monitor.sh $TARGET_DIR/scripts
	cp ${SCRIPT_PATH}/install.sh ${RELEASE_DIR}
	mkdir -p ${TARGET_DIR}/deps/
	cp ${FAISS_FILE} ${TARGET_DIR}/deps/
	cp ${BLAS_FILE} ${TARGET_DIR}/deps/
}

compress()
{
	cd ${RELEASE_DIR}
	tar zcvf queryServer.tar.gz queryServer
	echo "Compress done!"
}

gen_md5()
{
	md5=$(md5sum queryServer.tar.gz)
	md5=${md5:0:32}
	sed -i "s/MD5=0/MD5=${md5}/" install.sh
	echo "Generate md5 done!"
}

gen_prog()
{
	echo "===Binary files===" >> install.sh
	base64 queryServer.tar.gz >> install.sh

	mv install.sh queryServer_install.run
	chmod u+x queryServer_install.run

	echo "Generate installer done!"
}

clean()
{
	if [ -e "${RELEASE_DIR}/queryServer.tar.gz" ]; then
		echo "clean...."
		rm -rf ${TARGET_DIR}
		rm $RELEASE_DIR/queryServer.tar.gz
	fi
}

main()
{
	if [ $1 != 0 ];then
		BUILD_TYPE=1
	fi
	if [ $BUILD_TYPE == 1 ];
	then
		echo "Build type : release."
	else
		echo "Build type : debug"
	fi

	pre_check
	make_dir
	
	copy_files
	compress
	gen_md5
	gen_prog
	clean
}

main $# $@
