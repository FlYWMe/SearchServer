#! /bin/bash
source /etc/profile
# check every 30 seconds
# crontab -e
# * * * * * bash /home/hadoop/queryServer/scripts/monitor.sh
# * * * * * sleep 30; bash /home/hadoop/queryServer/scripts/monitor.sh

# 
PROCESS_NAME="queryServer"
PROCESS_PATH=$1 # 程序绝对路径
START_PROCESS="./${PROCESS_NAME}"
PROCESS_PARAMS=">>queryServer.log &"


# 函数: CheckProcess
# 功能: 检查一个进程是否存在
# 参数: $1 --- 要检查的进程名称
# 返回: 指定进程数量
#------------------------------------------------------------------------------
CheckProcess()
{
    #$PROCESS_NUM获取指定进程名的数目，为1返回0，表示正常，不为1返回1，表示有错误，需要重新启动
    PROCESS_NUM=`ps -A | grep "$1" | grep -v "grep" | wc -l` 
    return $PROCESS_NUM
}

# 检查实例是否已经存在
CheckProcess "${PROCESS_NAME}"
CheckRet=$?
if [ $CheckRet -eq 0 ]; then
    # 任意你需要执行的操作
    /bin/echo "$(date '+%Y-%m-%d %T') : Restarting ${PROCESS_NAME} ...">> ${PROCESS_PATH}/restart.log
    cd ${PROCESS_PATH}; 
    (${START_PROCESS}) >> ${PROCESS_PATH}/restart.log 2>&1 &
fi
