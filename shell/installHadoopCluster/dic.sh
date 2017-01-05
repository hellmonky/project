#!/bin/bash

declare -A IPListDict

function initIPListDict(){
    # read iplist.ini
    file=$1
    # get node ip
    source ${file}
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10 [node11]=$node11)
}

function initRoleListDict(){
    # read iplist.ini
    file=$1
    # get node ip
    ntp_master=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/ntp_master/)print $(i+1) }}'`
    hdfs_master=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/hdfs_master/)print $(i+1) }}'`
    hdfs_slave=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/hdfs_slave/)print $(i+1) }}'`
    zookeeper=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/zookeeper/)print $(i+1) }}'`
    hbase_master=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/hbase_master/)print $(i+1) }}'`
    hbase_slave=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/hbase_slave/)print $(i+1) }}'`
    kafka=`cat ${file} | awk -F '[=,{}]+' '{for(i=1;i<NF;i++){if($i~/kafka/)print $(i+1) }}'`

    RoleListDict=([ntp_master]=$ntp_master [hdfs_master]=$hdfs_master [hdfs_slave]=$hdfs_slave [zookeeper]=$zookeeper [hbase_master]=$hbase_master [hbase_slave]=$hbase_slave [kafka]=$kafka)
}

function initNodeRoleDict(){
    file=$1
    tmpfile=$2
    echo `cat $file |awk -F'=' '{split($2,arr,",");for(i=1;i<=length(arr);i++)z[arr[i]]=z[arr[i]]?z[arr[i]]","$1:$1}END{for(y in z)print y"="z[y]}'` >> $tmpfile
    source $tmpfile
    NodeRoleDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10 [node11]=$node11)
    rm -f $tmpfile
}



rolefile="/home/wentao/shell/installHadoopCluster/role.ini"
iplistfile="/home/wentao/shell/installHadoopCluster/iplist.ini"


initIPListDict ${iplistfile}
initRoleListDict ${rolefile}
initNodeRoleDict ${rolefile} ${tmpfile}


echo "查看IPListDict字典："
for k in "${!IPListDict[@]}"
do
    echo $k '--' ${IPListDict[$k]}
done

echo "查看RoleListDict字典："
for k in "${!RoleListDict[@]}"
do
    echo $k '--' ${RoleListDict[$k]}
done

echo "查看NodeRoleDict字典："
for k in "${!NodeRoleDict[@]}"
do
    echo $k '--' ${NodeRoleDict[$k]}
done

function getCurrentHOSTRole(){
    # 根据当前节点的HOST获取角色
    currentHostName=$1
    currentHostRole=${NodeRoleDict["$currentHostName"]}
    echo "${currentHostRole}"
}

HostName="node1"
HostRole=$(getCurrentHOSTRole ${HostName})
echo "当前节点的角色为：${HostRole}"