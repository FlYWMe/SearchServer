#!/bin/bash

PROGRAM_NAME="queryServer"
PACKAGE_NAME="${PROGRAM_NAME}.tar.gz"
MD5=0

check()
{

	if [ ! -e $PACKAGE_NAME ]; then
		echo "ERROR - can not find package file ${PACKAGE_NAME} !";
		exit 1;
	fi
	md5=$(md5sum $PACKAGE_NAME)
	md5=${md5:0:32}
	if [ "$MD5" != "$md5" ];then
		echo "check md5 failed! exit now!";
		exit 1;
	fi
}

extract()
{	
	echo "INFO - extracting ...."
	line=`awk '/===Binary files===/{print NR}' $0 | tail -n1` 
	line=`expr $line + 1`
	tail -n +$line $0 | base64 -d >${PACKAGE_NAME}

	check

	tar zxf $PACKAGE_NAME
	cd $PROGRAM_NAME
	chmod a+x ./*
	echo "INFO - unpack finished!"
}

install()
{

	echo "INFO - installing ..."
 
	sh ./scripts/install_deps.sh

	echo "INFO - install dependace finished!" 

	sh ./scripts/install_cuda.sh

}

post_install()
{
	#rm -rf ./deps
	rm -rf ./scripts
 
	rm ../$PACKAGE_NAME

	if [ -e "cuda-repo-ubuntu1404_8.0.61-1_amd64.deb" ]; then
		echo "----------------have cuda-repo-ubuntu1404"
		rm cuda-repo-ubuntu1404_8.0.61-1_amd64.deb	
	fi

	echo "INFO - Installation finished!"
}

setup_monitor()
{
	cp ./scripts/monitor.sh ./
	MONITOR="monitor"
	if [ ! -e ${MONITOR} ];then
		touch ${MONITOR}
	fi
	echo "* * * * * /bin/bash $(pwd)/monitor.sh $(pwd)" > ${MONITOR}
	crontab ${MONITOR}
	echo "INFO - Setup monitor done!"
}

main()
{
	extract
	install
	setup_monitor
	post_install
}

main
exit 0
