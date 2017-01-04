#!/bin/bash

##################### Hadoop Cluster Install ############################
############ author :   hellmonky
############ time   :   2017.01.03


# 注意：shell中关联数组需要声明才能正常使用，否则无法作为正确初始化
# 定义全局IPList字典（关联数组）
declare -A IPListDict
# 定义全局RoleList字典（关联数组）
declare -A RoleListDict
# 定义全局RoleList字典（关联数组）
declare -A NodeRoleDict


################ Stage 1: 获取配置文件内容 ################

# 获取字典内容：[Passing arrays as parameters in bash](http://stackoverflow.com/questions/1063347/passing-arrays-as-parameters-in-bash)
function getDictContent(){
    # 获取字典变量
    declare -a dic=("${!1}")

    echo "开始查询字典内容"

    #遍历key值
    for key in $(echo ${!dic[*]})
    do
        echo "$key : ${dic[$key]}"
    done

    echo "查询字典内容完毕"
}

# 获取字典内容
function getAssDictContent(){
    # 获取字典变量
    local -A dic
    eval "dic=(${!1})"

    echo "开始查询字典内容"

    #遍历key值
    for key in "${!dic[@]}"
    do
        echo "$key : ${dic[$k]}"
    done

    echo "查询字典内容完毕"
}


# 子字符串包含判断
function substr{
    STRING_A=$1
    STRING_B=$2
    if [[ ${STRING_A/${STRING_B}//} == $STRING_A ]]
    then
        ## is not substring.
        echo "A donot contains B"
        return 0
    else
        ## is substring.
        echo "A contains B"
        return 1
    fi
}

# 替换变量中的空格符为逗号分隔
function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}

# 初始化IPList字典
function initIPListDict(){
    # read iplist.ini
    file=$1
    # get node ip
    source ${file}
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10 [node11]=$node11)
}

# 初始化角色字典
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


# 初始化节点角色字典：
function initNodeRoleDict(){
    file=$1
    tmpfile=$2
    echo `cat $file |awk -F'=' '{split($2,arr,",");for(i=1;i<=length(arr);i++)z[arr[i]]=z[arr[i]]?z[arr[i]]","$1:$1}END{for(y in z)print y"="z[y]}'` >> $tmpfile
    source $tmpfile
    NodeRoleDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10 [node11]=$node11)
    rm -f $tmpfile
}

# 初始化节点角色字典：
function initNodeRoleDictVar(){
    
    # 获取ipdic中的节点名称，作为一个数组
    namelist=${!IPListDict[*]}
    echo $namelist
    
    # 然后遍历
    for k in "${!RoleListDict[@]}"
    do
        echo $k '--' ${RoleListDict[$k]}
    done

    #遍历当前role中的key值，获取
    for key in $(echo ${!RoleListDict[*]})
    do
        echo "$key : ${RoleListDict[$key]}"
        # 获取当前的值
        currentKey=$key
        currentValue=${RoleListDict[$key]}
        # 然后将当前的值中的逗号替换
        currentValue=${currentValue//,/ }
        # 进行遍历，将每一个节点角色追加到数组中
        for element in $currentValue
        do
            # 添加当前节点的角色到对应的数组中
            #if $element = node1_role
            echo $element
            #node1_role=("${node1_role[@]}" $currentKey) 
        done
    done

    # 最后将所有节点角色添加到角色字典中
    #NodeRoleDict+=([node1_role]=$node1_role)
    #NodeRoleDict+=([node2_role]=$node2_role)
    #NodeRoleDict+=([node3_role]=$node3_role)
    #NodeRoleDict+=([node4_role]=$node4_role)
    #NodeRoleDict+=([node5_role]=$node5_role)
    #NodeRoleDict+=([node6_role]=$node6_role)
    #NodeRoleDict+=([node7_role]=$node7_role)
    #NodeRoleDict+=([node8_role]=$node8_role)
    #NodeRoleDict+=([node9_role]=$node9_role)
}
# 需要使用其他字典来进行初始化
#initNodeRoleDict


# 根据输入修改IP
function changeHostIP(){
    eth=$1
    IP=$2

    # 修改IP地址
    echo "${eth} 内容如下："
    cat ${eth}

    # 修改输入修改ip地址
    sed -i "7a IPADDR=${IP}" ${eth}
    # 重启网卡
    systemctl restart  network.service
    # 
    echo "设置IP成功"
}

# 写入hosts文件：
function write2hosts(){
    source $1
    hostfile=$2

    # 
    i=1
    if [[ ${hdfs_master/","//} != $hdfs_master ]]
    then
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


################ Stage 2: 根据当前节点的角色进行设置 ################
# 获取本机的hostname：
function getCurrentHostName(){
    local_host="`hostname --fqdn`"
    echo ${local_host}
}

# 获取本机的IP：
function getCurrentIP(){
    local_ip=`ip addr | grep 'state UP' -A2 | tail -n1 | awk '{print $2}' | cut -f1  -d'/'`
    echo ${local_ip}
}

# 获取本机的角色：
function getCurrentHOSTRole(){
    # 根据当前节点的HOST获取角色
    currentHostName=$1
    currentHostRole=${NodeRoleDict[$currentHostName]}
    echo "${currentHostRole}"
}


# NTP服务端设置：
function ntp_master_config(){
    file=$1
    sed -i '8 c restrict default nomodify' ${file}
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;N;N;N;s/.*/server 127.127.1.0\nfudge  127.127.1.0 stratum 10/}' ${file}
    ntpd -c /etc/ntp.conf -p /tmp/ntpd.pid
    chkconfig ntpd on
    echo "完成NTP服务器设置"
}

# NTP节点设置
function ntp_slaver_config(){
    file=$1
    NTP_MASTER_IP=$2
    croncontent="*/1 * * * * /usr/sbin/ntpdate $NTP_MASTER_IP"
    cronfile="/var/spool/cron/root"
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;s/.*/server $NTP_MASTER_IP/}' ${file}
    # 创建定时任务
    touch ${cronfile}
    echo "${croncontent}" >> ${cronfile}
    service crond restart
    echo "完成NTP子节点设置"
}


# NTP服务部署入口：
function ntp_master_deploy(){
    currentHostRole=$(getCurrentHOSTRole)
    ntp_config_file=$1
    ntp_master_ip=$2

    echo ${currentHostRole}
    # 然后判断是否包含
    isContain=(substr ${currentHostRole} "NTP_MASTER")
    if [${isContain} == "1"]
    then
        ## NTP子节点：
        echo "ntp salve"
        ntp_slaver_config ${ntp_config_file} ${ntp_master_ip}
        return 0
    else
        ## NTP主节点：
        echo "ntp master"
        ntp_master_config ${ntp_config_file}
        return 1
    fi
}

# SSH到子节点，执行命令退出
function ssh_node(){
    USERNAME=$1
    HOSTS=$2
    SCRIPT="pwd; ls"
    for HOSTNAME in ${HOSTS} ; do
        ssh -l ${USERNAME} ${HOSTNAME} "${SCRIPT}"
    done
    return 1
}

# kafka设置
function setup_kafka(){
    # 获取对应的主机名
    ZOOKEEPER_NODES=$1
    echo "${ZOOKEEPER_NODES}"
    content=""
    for node in ${ZOOKEEPER_NODES} ; do
        content=${content}${node}":2181,"
    done
    length=`expr ${#content} - 1`
    content=${content:0:$length}
    # 修改文件命令，设置breaker的id为当前机器的hostname
    KAFKA_CONFIG=$2
    #sed -i "s/^zookeeper.connect=/zookeeper.connect=${content}/g" ${KAFKA_CONFIG}
    sed "/zookeeper.connect=localhost:2181/{s/.*/zookeeper.connect="${content}"/}" ${KAFKA_CONFIG}
}

# 安装tar格式MYSql
function installMySQL(){
    cd /usr/local/soft/
    tar zvxf mysql-5.7.10-linux-glibc2.5-i686.tar.gz -C /usr/local
    cd ..
    mv mysql-5.7.10-linux-glibc2.5-i686/ mysql
    
}



################ Stage 3: 组织完成安装部署的完整调用流程 ################
function run(){

    ################# 获取用户输入 #################
    # 输入当前节点要设置的IP地址：
    read -p "输入要设置的IP地址：" IP
    # 输入当前节点中role.ini文件所在的位置
    read -p "输入role.ini文件所在的位置：" rolefile
    # 输入当前节点中iplist.ini文件所在的位置
    read -p "输入iplist.ini文件所在的位置：" iplistfile

    ################# 确认用户输入 #################
    echo ${IP}
    echo ${rolefile}
    echo ${iplistfile}

    ################# 初始化全局设置 #################
    # 初始化IPListDict：
    initIPListDict ${IPListFile}
    #打印指定key的value
    echo ${IPListDict["node1"]}
    #打印所有key值
    echo ${!IPListDict[*]}
    #打印所有value
    echo ${IPListDict[*]}

    # 初始化RoleListDict:
    initRoleListDict ${rolefile}
    #打印指定key的value
    echo ${RoleListDict["ntp_master"]}
    #打印所有key值
    echo ${!RoleListDict[*]}
    #打印所有value
    echo ${RoleListDict[*]}

    # 初始化NodeRoleDict：
    # 设置临时文件路径
    tmpfile="/home/wentao/shell/my.ini"
    initNodeRoleDict ${rolefile} ${tmpfile}

    # 查看字典初始化结果：传入关联数组作为参数
    getDictContent IPListDict[@]
    getDictContent RoleListDict[@]
    getDictContent NodeRoleDict[@]
    for k in "${!NodeRoleDict[@]}"
    do
        echo $k '--' ${NodeRoleDict[$k]}
    done


    ################# 开始进行调用 #################
    # （1）根据当前用户的输入，设置本机IP
    # 设置网卡配置文件路径
    ifcfg_file='/home/wentao/shell/ifcfg-ens33'
    changeHostIP ${ifcfg_file} ${IP}
    #changeHostIP '/etc/sysconfig/network-scripts/ifcfg-ens18' '192.168.1.100'

    # 根据用户指定的iplist.ini文件，设置hosts
    # 设置当前hosts文件路径
    hostsfile='/home/wentao/shell/hosts'
    write2hosts ${rolefile} ${hostsfile}
    #write2hosts '/home/wentao/shell/role.ini' '/etc/hosts'

    # 获取当前角色:
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName} ${HostIP})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"

    # 根据当前角色设置NTP：
    # 设置ntp服务的配置文件
    ntp_config_file="/home/wentao/shell/ntp_slave.conf"
    # 根据role.ini获取ntp服务器的ip
    ntp_master_node=${RoleListDict["ntp_master"]}
    ntp_master_ip=${IPListDict[$ntp_master_node]}
    ntp_master_deploy ${ntp_config_file} ${ntp_master_ip}

    # 根据当前角色进行ssh测试：
    # 默认用户名都是root
    ssh_user='root'
    # 获取IPListDict的所有值进行连接测试
    host=${!IPListDict[*]}
    # 需要除去自己当前的ip，否则无法连接成功
    host=(${host[@]/${IP}/})
    # 开始测试，如果测试正常将返回1
    ssh_result=$(ssh_node ${ssh_user} ${host})
    if [ssh_result == "1"]
    then
        echo "SSH链接测试成功"
    else
        echo "SSH链接测试失败"
    fi

    # 根据当前角色安装HDFS：
    # 获取hdfs_master节点列表
    hdfs_master_node=${RoleListDict["hdfs_master"]}
    # 获取hdfs_second_master节点列表
    hdfs_second_master_node=""
    # 获取hdfs_salve节点列表
    hdfs_slave_node=${RoleListDict["hdfs_slave"]}
    # 判断当前是否需要安装HDFS服务,HostRole是当前角色
    isHDFS_master=(substr ${hdfs_master_node} ${HostRole})
    isHDFS_salve=(substr ${hdfs_slave_node} ${HostRole})
    if [${isHDFS_master} == "1" || ${isHDFS_salve} == "1" ]
    then
        echo "执行HDFS安装部署"
        # 替换空格为逗号
        hdfs_slave_nodes=(replaceBlank2Comma ${hdfs_slave_node})
        # 部署HDFS服务：
        sh hadoop-install.sh -m ${hdfs_master_node} -n ${hdfs_second_master_node} -s ${hdfs_slave_nodes}
    else
        echo "不需要执行HDFS安装部署"
    fi

    # 根据当前角色安装HBase：
    # 获取zookeeper节点列表
    zookeeper_node=${RoleListDict["zookeeper"]}
    # 获取hbase_master节点列表
    hbase_master_node=${RoleListDict["hbase_master"]}
    # 获取hbase_slave节点列表
    hbase_slave_node=${RoleListDict["hbase_slave"]}
    # 判断当前是否需要安装HDFS服务,HostRole是当前角色
    isHBase_master=(substr ${hbase_master_node} ${HostRole})
    isHBase_salve=(substr ${hbase_slave_node} ${HostRole})
    if [${isHBase_master} == "1" || ${isHBase_salve} == "1" ]
    then
        echo "执行HBase安装部署"
        # 替换空格为逗号
        hbase_nodes=(replaceBlank2Comma ${hbase_slave_node})
        zookeeper_nodes=(replaceBlank2Comma ${zookeeper_node})
        # 部署HBase服务：
        sh hbase-install.sh -m ${hbase_master_node} -s ${hbase_nodes} -z ${zookeeper_nodes}
    else
        echo "不需要执行HBase安装部署"
    fi

    # 根据当前角色安装zookeeper：
    sh zookeeper-install.sh -q ${zookeeper_nodes}

    # 安装kafka：
    # 从role.ini获取kafka对应的节点列表
    kafka_nodes=${RoleListDict["kafka"]}
    # 判断当前是否需要安装kafka
    isKafka=(substr ${kafka_nodes} ${HostRole})
    if [ ${isKafka} == "1"]
    then
        echo "执行kafka安装部署" 
        # 设置kafka的配置文件路径
        kafka_config='server.properties'
        # 设置kafka配置
        setup_kafka "${zookeeper_node}" ${kafka_config}
    else
        echo "不需要执行kafka安装部署"
    fi
    
    ################# 开始进行调用 #################
    echo "当前节点安装部署结束"
}