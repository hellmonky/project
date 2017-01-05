#!/bin/bash

declare -A IPListDict
declare -A RoleListDict
declare -A NodeRoleDict

function getDictContent(){
    declare -a dic=("${!1}")

    echo "start check dict"

    for key in $(echo ${!dic[*]})
    do
        echo "$key : ${dic[$key]}"
    done

    echo "end check dict"
}

function getAssDictContent(){
    local -A dic
    eval "dic=(${!1})"

    echo "start check dict"

    for key in "${!dic[@]}"
    do
        echo "$key : ${dic[$k]}"
    done

    echo "end check dict"
}

function substr(){
    STRING_A=$1
    STRING_B=$2

    if [[ ${STRING_A/${STRING_B}//} == $STRING_A ]]
    then
        ## is not substring.
        echo N
        return 0
    else
        ## is substring.
        echo Y
        return 1
    fi
}

function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}

function installMySQL(){
    tarfilepath=$1
    desfilepath=$2
    filename=$3 
    tar zvxf ${tarfilepath} -C ${desfilepath}
    cd ${desfilepath}
    mv ${filename}/ mysql
}

function installJDK(){
    tarfilepath=$1
    tarfoldername=$2
    destfilepath=$3
    destfolderename=$4
    sysintfile=$5
    
    tar zvxf ${tarfilepath} -C ${destfilepath}
    cd ${destfilepath}
    mv ${tarfoldername}/ ${destfolderename}

    echo "export JAVA_HOME=${destfilepath}/${destfolderename}" >> ${sysintfile}
    echo "export PATH=$PATH:$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH" >> ${sysintfile}
    echo "export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib" >> ${sysintfile}

    source ${sysintfile}
}



function initIPListDict(){
    file=$1
    source ${file}
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10 [node11]=$node11)
}

function initRoleListDict(){
    file=$1
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
    #rm -f $tmpfile
}

function initNodeRoleDictVar(){
    
    namelist=${!IPListDict[*]}
    echo $namelist
    
    for k in "${!RoleListDict[@]}"
    do
        echo $k '--' ${RoleListDict[$k]}
    done

    for key in $(echo ${!RoleListDict[*]})
    do
        echo "$key : ${RoleListDict[$key]}"
        currentKey=$key
        currentValue=${RoleListDict[$key]}
        currentValue=${currentValue//,/ }
        for element in $currentValue
        do
            echo $element
        done
    done
}



function changeHostIP(){
    eth=$1
    IP=$2

    echo "${eth} is:"
    cat ${eth}
    sed -i "7a IPADDR=${IP}" ${eth}
    systemctl restart  network.service

    echo "set IP success"
}

function write2hosts(){
    source $1
    hostfile=$2

    if [[ ${hdfs_master/","//} != $hdfs_master ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $hdfs_master|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "hdfs_master     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "hdfs_master        " ${IPListDict["$hdfs_master"]} >> ${hostfile}
    fi


    if [[ ${ntp_master/","//} != $ntp_master ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $ntp_master|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "ntp_master     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "ntp_master        " ${IPListDict["$ntp_master"]} >> ${hostfile}
    fi


    if [[ ${hdfs_slave/","//} != $hdfs_slave ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $hdfs_slave|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "hdfs_slave     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "hdfs_slave        " ${IPListDict["$hdfs_slave"]} >> ${hostfile}
    fi


    if [[ ${zookeeper/","//} != $zookeeper ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $zookeeper|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "zookeeper     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "zookeeper        " ${IPListDict["$zookeeper"]} >> ${hostfile}
    fi



    if [[ ${hbase_master/","//} != $hbase_master ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $hbase_master|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "hbase_master     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "hbase_master        " ${IPListDict["$hbase_master"]} >> ${hostfile}
    fi



    if [[ ${hbase_slave/","//} != $hbase_slave ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $hbase_slave|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "hbase_slave     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "hbase_slave        " ${IPListDict["$hbase_slave"]} >> ${hostfile}
    fi



    if [[ ${kafka/","//} != $kafka ]]
    then
        i=1
        while((1==1))
        do
            split=`echo $kafka|cut -d "," -f$i`
            if [ "$split" != "" ]
            then
                ((i++))
                echo "kafka     " ${IPListDict["$split"]} >> ${hostfile}
            else
                break
            fi
        done
    else
        echo "kafka        " ${IPListDict["$kafka"]} >> ${hostfile}
    fi
}


function getCurrentHostName(){
    local_host="`hostname --fqdn`"
    echo ${local_host}
}

function getCurrentIP(){
    local_ip=`ip addr | grep 'state UP' -A2 | tail -n1 | awk '{print $2}' | cut -f1  -d'/'`
    echo ${local_ip}
}

function getCurrentHOSTRole(){
    currentHostName=$1
    currentHostRole=${NodeRoleDict[$currentHostName]}
    echo "${currentHostRole}"
}

function ntp_master_config(){
    file=$1
    sed -i '8 c restrict default nomodify' ${file}
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;N;N;N;s/.*/server 127.127.1.0\nfudge  127.127.1.0 stratum 10/}' ${file}
    ntpd -c /etc/ntp.conf -p /tmp/ntpd.pid
    chkconfig ntpd on
    echo "NTP server setup success"
}

function ntp_slaver_config(){
    file=$1
    NTP_MASTER_IP=$2
    croncontent="*/1 * * * * /usr/sbin/ntpdate $NTP_MASTER_IP"
    cronfile="/var/spool/cron/root"
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;s/.*/server $NTP_MASTER_IP/}' ${file}

    touch ${cronfile}
    echo "${croncontent}" >> ${cronfile}
    service crond restart
    echo "NTP client setup success"
}

function ntp_master_deploy(){
    currentHostRole=$(getCurrentHOSTRole)
    ntp_config_file=$1
    ntp_master_ip=$2

    echo ${currentHostRole}
    isContain=(substr ${currentHostRole} "NTP_MASTER")
    if [${isContain} == "1"]
    then
        echo "ntp salve"
        ntp_slaver_config ${ntp_config_file} ${ntp_master_ip}
        return 0
    else
        echo "ntp master"
        ntp_master_config ${ntp_config_file}
        return 1
    fi
}

function ssh_node(){
    USERNAME=$1
    HOSTS=$2
    SCRIPT="pwd; ls"
    for HOSTNAME in ${HOSTS}
    do
        echo "current ssh to : ${HOSTNAME}"
        ssh -l ${USERNAME} ${HOSTNAME} "${SCRIPT}"
    done
    return 1
}

function setup_kafka(){
    ZOOKEEPER_NODES=$1
    echo "${ZOOKEEPER_NODES}"
    content=""
    for node in ${ZOOKEEPER_NODES} ; do
        content=${content}${node}":2181,"
    done
    length=`expr ${#content} - 1`
    content=${content:0:$length}
    
    KAFKA_CONFIG=$2
    sed "/zookeeper.connect=localhost:2181/{s/.*/zookeeper.connect="${content}"/}" ${KAFKA_CONFIG}
}


function testInit(){
    rm -f /home/wentao/shell/dest/*

    cp /etc/sysconfig/network-scripts/ifcfg-ens33 /home/wentao/shell/dest/
    cp /etc/hosts /home/wentao/shell/dest/
    cp /etc/ntp.conf /home/wentao/shell/dest/
    cp /etc/profile /home/wentao/shell/dest/
    cp /home/wentao/shell/tmp/server.properties.bak /home/wentao/shell/dest/server.properties
    
    HostName=$(getCurrentHostName)
    echo "current hostname : ${HostName}"
    new_hostname=$1
    hostname ${new_hostname}
    echo "change currenthostname to : ${new_hostname}"

    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "hostname change success"
    echo "current IP : ${HostIP}"
    echo "current hostname is : ${HostName}"
    echo "current host role is : ${HostRole}"
}


function run(){

    ifcfg_file='/home/wentao/shell/dest/ifcfg-ens33'
    hostsfile='/home/wentao/shell/dest/hosts'
    ntp_config_file="/home/wentao/shell/dest/ntp.conf"
    ssh_user='root'
    tarfilepath="/home/wentao/shell/package/jdk-8u112-linux-x64.tar.gz"
    tarfoldername="jdk1.8.0_112"
    destfilepath="/home/wentao/shell/dest/"
    destfolderename="JDK"
    sysintfile="/home/wentao/shell/dest/profile"
    tmpfile="/home/wentao/shell/dest/my.ini"
    kafka_config='/home/wentao/shell/dest/server.properties'
    


    #read -p "setup ip : " IP
    #read -p "input role.ini path : " rolefile
    #read -p "input iplist.ini path : " iplistfile

    IP='192.168.15.133'
    rolefile="/home/wentao/shell/installHadoopCluster/role.ini"
    iplistfile="/home/wentao/shell/installHadoopCluster/iplist.ini"


    echo ${IP}
    echo ${rolefile}
    echo ${iplistfile}


    initIPListDict ${IPListFile}
    initRoleListDict ${rolefile}
    initNodeRoleDict ${rolefile} ${tmpfile}

    getDictContent IPListDict[@]
    getDictContent RoleListDict[@]
    getDictContent NodeRoleDict[@]


    installJDK ${tarfilepath} ${tarfoldername} ${destfilepath} ${destfolderename} ${sysintfile}


    changeHostIP ${ifcfg_file} ${IP}
    write2hosts ${rolefile} ${hostsfile}

    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "current IP : ${HostIP}"
    echo "current hostname is : ${HostName}"
    echo "current host role is : ${HostRole}"

    ntp_master_node=${RoleListDict["ntp_master"]}
    ntp_master_ip=${IPListDict[$ntp_master_node]}
    ntp_master_deploy ${ntp_config_file} ${ntp_master_ip}

    host=${!IPListDict[*]}
    host=(${host[@]/${IP}/})
    ssh_result=$(ssh_node ${ssh_user} ${host})
    if [ssh_result == "1"]
    then
        echo "SSH test success"
    else
        echo "SSH test fail"
    fi

    hdfs_master_node=${RoleListDict["hdfs_master"]}
    hdfs_second_master_node=""
    hdfs_slave_node=${RoleListDict["hdfs_slave"]}
    isHDFS_master=(substr ${hdfs_master_node} ${HostRole})
    isHDFS_salve=(substr ${hdfs_slave_node} ${HostRole})
    if [[${isHDFS_master} == "1" || ${isHDFS_salve} == "1" ]]
    then
        echo "HDFS install start"
        hdfs_slave_nodes=(replaceBlank2Comma ${hdfs_slave_node})
        sh hadoop-install.sh -m ${hdfs_master_node} -n ${hdfs_second_master_node} -s ${hdfs_slave_nodes}
    else
        echo "HDFS donot need to install"
    fi

    zookeeper_node=${RoleListDict["zookeeper"]}
    hbase_master_node=${RoleListDict["hbase_master"]}
    hbase_slave_node=${RoleListDict["hbase_slave"]}
    isHBase_master=(substr ${hbase_master_node} ${HostRole})
    isHBase_salve=(substr ${hbase_slave_node} ${HostRole})
    if [[${isHBase_master} == "1" || ${isHBase_salve} == "1" ]]
    then
        echo "HBase install start"
        hbase_nodes=(replaceBlank2Comma ${hbase_slave_node})
        zookeeper_nodes=(replaceBlank2Comma ${zookeeper_node})
        sh hbase-install.sh -m ${hbase_master_node} -s ${hbase_nodes} -z ${zookeeper_nodes}
    else
        echo "HBase donot need to install"
    fi

    sh zookeeper-install.sh -q ${zookeeper_nodes}

    kafka_nodes=${RoleListDict["kafka"]}
    isKafka=(substr ${kafka_nodes} ${HostRole})
    if [ ${isKafka} == "1"]
    then
        echo "kafka install start"
        setup_kafka "${zookeeper_node}" ${kafka_config}
    else
        echo "kafka donot need to install"
    fi
    
    echo "finish node install"
}


testInit "node1"
run
