#!/bin/bash

declare -A IPListDict
declare -A RoleListDict
declare -A NodeRoleDict
function getDictContent(){
    declare -a dic=("${!1}")
    echo "开始查询字典内容"
    for key in $(echo ${!dic[*]})
    do
        echo "$key : ${dic[$key]}"
    done
    echo "查询字典内容完毕"
}
function getAssDictContent(){
    local -A dic
    eval "dic=(${!1})"
    echo "开始查询字典内容"
    for key in "${!dic[@]}"
    do
        echo "$key : ${dic[$k]}"
    done
    echo "查询字典内容完毕"
}
function substr(){
    STRING_A=$1
    STRING_B=$2
    if [[ ${STRING_A/${STRING_B}//} == $STRING_A ]]
    then
        echo 0
    else
        echo 1
    fi
}
function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}
function replaceComma2Blank(){
    context="$@"
    echo "$context"|sed "s/,/ /g"
}
function releaseTarPackage(){
    tarfilepath=$1
    desfilepath=$2
    tar -zxf ${tarfilepath} -C ${desfilepath}
}
function installJDK(){
    tarfilepath=$1
    tarfoldername=$2
    destfilepath=$3
    destfolderename=$4
    eval "tar -zxf ${tarfilepath} -C ${destfilepath}"
    mv ${destfilepath}/${tarfoldername}/ ${destfilepath}/${destfolderename}
    sysintfile="/etc/profile"
    echo "export JAVA_HOME=${destfilepath}/${destfolderename}" >> ${sysintfile}
    echo "export PATH=$PATH:$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH" >> ${sysintfile}
    echo "export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib" >> ${sysintfile}
    eval "source ${sysintfile}"
}
function installMySQL(){
    tarfilepath=$1
    desfilepath=$2
    filename=$3
    cnfpath=$4
    tar -zxf ${tarfilepath} -C ${desfilepath}
    mv ${desfilepath}/${filename} ${desfilepath}/mysql
    groupadd mysql
    useradd -r -g mysql mysql
    chown -R mysql:mysql ${desfilepath}/mysql
    ${desfilepath}/mysql/bin/mysqld --initialize-insecure --user=mysql  
	
    cp  ${cnfpath} /etc/my.cnf 
}
function startMySQL(){
	 /usr/local/mysql/support-files/mysql.server start
}
function stopMySQL(){
	 /usr/local/mysql/support-files/mysql.server stop
}
function getPID_MySQL(){
        bool=` ps -ef | grep mysql |grep -v grep | grep -v sh |awk '{print $2}' `
        if [  "$bool" = "" ];then
           echo "MySQL is not running"
           return 0
        else
           echo "MySQL is running"
           return ${bool}
        fi
}
function addRomoteLoginUser(){
    mysqllogin="root"
    mysqlpass="" 
    newusername="iscas"
    newuserpass="12345"
    sql_createuser="CREATE USER '${newusername}' IDENTIFIED BY '${newuserpass}';";
    sql_grant1="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'%';";
    sql_grant2="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'localhost';";
    sql_add="${sql_createuser}${sql_grant1}${sql_grant2}";
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}
function deleteuser(){
    delusername="iscas"
    mysqllogin="root" 
    mysqlpass="" 
    sql_del1="DROP user '${delusername}'@'%';";
    sql_del2="DROP user '${delusername}'@localhost;";
    sql_add="${sql_del1}";
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}
function rootUserLogin(){
    mysqllogin="root"
    mysqlpass=""
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass ;
}
function createtable(){
    mysqllogin="iscas" 
    mysqlpass="12345" 
    createdb="test";   
    location="/usr/local/componetmetadata.sql";
    ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
    sql_db="Create Database if Not Exists ${createdb} character set utf8 ;";
    select_db="use ${createdb};";
    sql_loc="source ${location};";
    sql_add="${sql_db}${select_db}${sql_loc}";
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass  -h$ipaddr --execute="$sql_add";
}
function localhostlogin(){
    ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
    mysqllogin="iscas"
    mysqlpass="12345"
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass -h$ipaddr;
}
function releaseES(){
    es_tarpath='/usr/local/package/elasticsearch-1.6.0.tar.gz'
    desfilepath='/usr/local/'   
    foldername='elasticsearch-1.6.0'
    releaseTarPackage ${es_tarpath} ${desfilepath}
    chmod -R 775 ${desfilepath}/${foldername}
}
function updateES(){
   nodename=$1
   FILE=/usr/local/elasticsearch-1.6.0/config/elasticsearch.yml
   KEY='node.name: "node_es1"'
   INSTEAD='node.name: '${nodename}''
   sed -i  's/'"$KEY"'/'"$INSTEAD"'/g'  $FILE
}
function installES(){
    currentHostname=$1
    releaseES
    updateES ${currentHostname}
}
function startES(){  
    /usr/local/elasticsearch-1.6.0/bin/elasticsearch  -d
}
function stopES(){
    ps -ef | grep elasticsearch | grep -v grep | awk '{print $2}'| xargs kill
}
function installfilebeat(){
    fb_tarpath='/usr/local/package/filebeat-1.0.0-x86_64.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${fb_tarpath} -C ${desfilepath}
}
function updatefilebeat(){
    currentHostname=$1
    currentIP=$2
   fb_path='/usr/local/package/filebeat.yml'
   cp ${fb_path}  /usr/local/filebeat-1.0.0-x86_64/
   FILE=/usr/local/filebeat-1.0.0-x86_64/filebeat.yml
   nodename=${currentHostname}
   nodeip=${currentIP}
   KEY1='#name:'
   INSTEAD1='name: '${nodename}''
   sed -i  's/'"$KEY1"'/'"$INSTEAD1"'/g'  $FILE
   KEY2='localhost:'
   INSTEAD2=''${nodeip}':'
   sed -i  's/'"$KEY2"'/'"$INSTEAD2"'/g'  $FILE
}
function startfilebeat(){
    /usr/local/filebeat-1.0.0-x86_64/filebeat -e -c /usr/local/filebeat-1.0.0-x86_64/filebeat.yml   &
}
function stopfilebeat(){
    ps -ef | grep filebeat | grep -v grep | awk '{print $2}'| xargs kill
}
function installlogstash(){
    log_tarpath='/usr/local/package/logstash-1.5.6.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${log_tarpath} -C ${desfilepath}
}
function updatelogstash(){
    ipaddr=$1
   ls_path='/usr/local/package/beatsfinal.conf'
   cp ${ls_path}  /usr/local/logstash-1.5.6/
   FILE=/usr/local/logstash-1.5.6/beatsfinal.conf
   KEY="0.0.0.0"
   sed -i  's/'"$KEY"'/'"${ipaddr}"'/g'  $FILE
}
function startlogstash(){
    /usr/local/logstash-1.5.6/bin/logstash -f /usr/local/logstash-1.5.6/beatsfinal.conf &
}
function stoplogstash(){
    ps -ef | grep logstash | grep -v grep | awk '{print $2}'| xargs kill
}
function installkibana(){
    kb_tarpath='/usr/local/package/kibana-4.0.3-linux-x64.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${kb_tarpath} -C ${desfilepath}  
}
function updatekibana(){
    ipaddr=$1
   FILE=/usr/local/kibana-4.0.3-linux-x64/config/kibana.yml
   KEY1="0.0.0.0"
   KEY2="localhost:"
   sed -i  's/'"$KEY1"'/'"$ipaddr"'/g'  $FILE
   sed -i  's/'"$KEY2"'/'"$ipaddr":'/g'  $FILE
}
function startkibana(){
    /usr/local/kibana-4.0.3-linux-x64/bin/kibana &
}
function stopkibana(){
    ps -ef | grep kibana | grep -v grep | awk '{print $2}'| xargs kill
}
function installKafka(){
    ZOOKEEPER_NODES=$1
    ZOOKEEPER_NODES=$(replaceComma2Blank ${ZOOKEEPER_NODES})
    content=""
    for node in ${ZOOKEEPER_NODES} ; do
        content=${content}${node}":2181,"
    done
    length=`expr ${#content} - 1`
    content=${content:0:$length}
    KAFKA_CONFIG=$2
    sed -i "/zookeeper.connect=localhost:2181/{s/.*/zookeeper.connect="${content}"/}" ${KAFKA_CONFIG}
}
function startupKafka(){
    kafka_root_path=$1
    ${kafka_root_path}/bin/kafka-server-start.sh ${kafka_root_path}/config/server.properties
}
function stopKafka(){
    kafka_root_path=$1
    ${kafka_root_path}/bin/kafka-server-stop.sh
    return 0
}
function startup_Cluster_kafka(){
    zookeeper_nodes_List="kfq1 kfq2 kfq3 kfq4 kfq5 kfq6"  
    kafka_root_path="/usr/local/hadoop/kafka_2.11-0.9.0.1/"  
    kafka_version=kafka_2.11-0.9.0.1
    for i in ${zookeeper_nodes_List}
    do
        echo "Starting ${kafka_version} on ${i} "
        ssh ${i} "bash ${kafka_root_path}/bin/kafka-server-start.sh ${kafka_root_path}/config/server.properties"
        if [[ $? -ne 0 ]]; then
            echo "Starting ${kafka_version} on ${i} success"
        fi
    done
    echo All ${kafka_version} are started
    return 0
}
function stop_Cluster_kafka(){
    zookeeper_nodes_List="kfq1 kfq2 kfq3 kfq4 kfq5 kfq6"  
    kafka_root_path="/usr/local/hadoop/kafka_2.11-0.9.0.1/"  
    kafka_version=kafka_2.11-0.9.0.1
    for i in ${zookeeper_nodes_List}
    do
        echo "Stopping ${kafka_version} on ${i} "
        ssh ${i} "bash ${kafka_root_path}/bin/kafka-server-stop.sh"
        if [[ $? -ne 0 ]]; then
            echo "Stopping ${kafka_version} on ${i} success"
        fi
    done
    echo All ${kafka_version} are stoped
    return 0
}
function install_node_zookeeper(){
    zookeeper_nodes_List=$1
    sh zookeeper-install.sh -q ${zookeeper_nodes_List}
}
function startup_Cluster_zookeeper(){
    zookeeper_nodes_List=$1
    zookeeper_nodes_List=$(replaceComma2Blank ${zookeeper_nodes_List})
    zookeeper_root_path="/usr/local/hadoop/zookeeper-3.4.6/"  
    zookeeper_version=zookeeper-3.4.6
    for i in ${zookeeper_nodes_List}
    do
        echo "Starting ${zookeeper_version} on ${i} "
        ssh ${i} "bash ${zookeeper_root_path}/bin/zkServer.sh start"
        if [[ $? -ne 0 ]]; then
            echo "Starting ${zookeeper_version} on ${i} success"
        fi
    done
    echo All ${zookeeper_nodes_List} are started
    return 0
}
function stop_Cluster_zookeeper(){
    zookeeper_nodes_List=$1
    zookeeper_nodes_List=$(replaceComma2Blank ${zookeeper_nodes_List})
    zookeeper_root_path="/usr/local/hadoop/zookeeper-3.4.6/" 
    zookeeper_version=zookeeper-3.4.6
    for i in ${zookeeper_nodes_List}
    do
        echo "Stopping ${zookeeper_version} on ${i} "
        ssh ${i} "bash ${zookeeper_root_path}/bin/zkServer.sh stop"
        if [[ $? -ne 0 ]]; then
            echo "Stopping ${zookeeper_version} on ${i} success"
        fi
    done
    echo All ${zookeeper_nodes_List} are stopped
    return 0
}
function check_zookeeper(){
    zookeeper_pid=`jps | grep 'QuorumPeerMain' | cut -d ' ' -f 1`
    if [ ! "${zookeeper_pid}" ]; then
        echo 0
    else
        echo ${zookeeper_pid}
    fi
}
function hbase_node_install(){
    hbase_master_node=$1
    hbase_nodes=$2
    zookeeper_nodes=$3
    sh hbase-install.sh -m ${hbase_master_node} -s ${hbase_nodes} -z ${zookeeper_nodes}
}
function hbase_master_startup(){
    /usr/local/hadoop/hbase-1.2.0/bin/start-hbase.sh
}
function hbase_master_stop(){
    /usr/local/hadoop/hbase-1.2.0/bin/stop-hbase.sh
}
function check_hbase(){
    hbase_pid=`jps | grep 'HMaster' | cut -d ' ' -f 1`
    if [ ! "${hbase_pid}" ]; then
        hbase_pid=`jps | grep 'HRegionServer' | cut -d ' ' -f 1`
            if [ ! "${hbase_pid}" ]; then
                echo 0
            else
                echo ${hbase_pid}
            fi
    else
        echo ${hbase_pid}
    fi
}
function hdfs_node_install(){
    hdfs_master_node=$1
    hdfs_second_master_node=$2
    hdfs_slave_nodes=$3
    sh hadoop-install.sh -m ${hdfs_master_node} -n ${hdfs_second_master_node} -s ${hdfs_slave_nodes}
}
function hdfs_master_startup(){
    /usr/local/hadoop/hadoop-2.6.0/sbin/start-dfs.sh
    /usr/local/hadoop/hadoop-2.6.0/sbin/start-yarn.sh
}
function hdfs_master_stop(){
    /usr/local/hadoop/hadoop-2.6.0/sbin/stop-dfs.sh
    /usr/local/hadoop/hadoop-2.6.0/sbin/stop-yarn.sh
}
function check_hdfs(){
    hdfs_pid=`jps | grep -w 'NameNode' | cut -d ' ' -f 1`
    if [ ! "${hdfs_pid}" ]; then
        hdfs_pid=`jps | grep -w 'DataNode' | cut -d ' ' -f 1`
            if [ ! "${hdfs_pid}" ]; then
                echo 0
            else
                echo ${hdfs_pid}
            fi
    else
        echo ${hdfs_pid}
    fi
}
function normalizdinifile(){
    rolefile=$1
    iplistfile=$2
    tmpfile=$3
    cat -v ${rolefile} | tr -d '^M'  > ${tmpfile}
    rm -f ${rolefile}
    mv ${tmpfile} role.ini
    cat -v ${iplistfile} | tr -d '^M'  > ${tmpfile}
    rm -f ${iplistfile}
    mv ${tmpfile} iplist.ini
}
function initIPListDict(){
    file=$1
    eval "source ${file}"
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10)
}
function initRoleListDict(){
    file=$1
    eval "source ${file}"
    RoleListDict=([ntp_master]=$ntp_master [hdfs_master]=$hdfs_master [hdfs_slave]=$hdfs_slave [zookeeper]=$zookeeper [hbase_master]=$hbase_master [hbase_slave]=$hbase_slave [kafka]=$kafka [mysql]=$mysql [es]=${es} [filebeat]=${filebeat} [logstash]=${logstash} [kibana]=${kibana})
}
function initNodeRoleDict(){
    file=$1
    tmpfile=$2
    echo `cat $file |awk -F'=' '{split($2,arr,",");for(i=1;i<=length(arr);i++)z[arr[i]]=z[arr[i]]?z[arr[i]]","$1:$1}END{for(y in z)print y"="z[y]}'` >> $tmpfile
    eval "source $tmpfile"
    NodeRoleDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10)
    rm -f $tmpfile
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
    echo "要修改的网卡配置文件为："
    echo ${eth}
    sed -i "13c IPADDR=${IP}" ${eth}
    systemctl restart  network.service
    result=`echo $?`
    echo ${result}
    if [ ${result} == "0" ]
    then
        echo "设置IP成功"
        echo "修改后的文件内容为："
        cat ${eth}
    else
        echo "设置IP失败，请查看是否有权限"
    fi
}
function write2hosts(){
    iplist_file=$1
    hostfile=$2
    source ${iplist_file}
    echo "$node1"" node1" >> ${hostfile}
    echo "$node2"" node2" >> ${hostfile}
    echo "$node3"" node3" >> ${hostfile}
    echo "$node4"" node4" >> ${hostfile}
    echo "$node5"" node5" >> ${hostfile}
    echo "$node6"" node6" >> ${hostfile}
    echo "$node7"" node7" >> ${hostfile}
    echo "$node8"" node8" >> ${hostfile}
    echo "$node9"" node9" >> ${hostfile}
    echo "$node10"" node10" >> ${hostfile}
    systemctl restart network.service
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
    currentHostRole=${NodeRoleDict["$currentHostName"]}
    echo "${currentHostRole}"
}
function ntp_master_config(){
    file=$1
    sed -i '8 c restrict default nomodify' ${file}
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;N;N;N;s/.*/server 127.127.1.0\nfudge  127.127.1.0 stratum 10/}' ${file}
    ntpd -c /etc/ntp.conf -p /tmp/ntpd.pid
    chkconfig ntpd on
    echo "完成NTP服务器设置"
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
    echo "完成NTP子节点设置"
}
function ntp_master_deploy(){
    currenthostname=$1
    ntp_config_file=$2
    ntp_master_ip=$3
    currentHostRole=$(getCurrentHOSTRole ${currenthostname})
    isContain=$(substr ${currentHostRole} "ntp_master")
    if [ ${isContain} == "1" ]
    then
        echo "ntp master detected"
        ntp_master_config ${ntp_config_file}
        return 1
    else
        echo "ntp salve detected"
        ntp_slaver_config ${ntp_config_file} ${ntp_master_ip}
        return 0
    fi
}
function ssh2nodes(){
    username=$1
    iplist=$2
    script="pwd; ls"
    val=1
    for currentip in ${iplist}
    do
        echo "current ssh to : ${currentip}"
        local RESULTS
        RESULTS=$(ssh -l ${username} ${currentip} "${script}")
        tmp=`echo $?`
        if [ "$tmp" == 0 ]
        then
            echo "当前节点SSH ${currentip} 成功"
        else
            val=0
            break
        fi
    done
    return "$val"
}
function testInit(){
    rm -rf /home/wentao/shell/dest/*
    cp /etc/sysconfig/network-scripts/ifcfg-ens33 /home/wentao/shell/dest/
    cp /etc/hosts /home/wentao/shell/dest/
    cp /etc/ntp.conf /home/wentao/shell/dest/
    cp /etc/profile /home/wentao/shell/dest/
    cp /home/wentao/shell/tmp/server.properties.bak /home/wentao/shell/dest/server.properties
    HostName=$(getCurrentHostName)
    echo "当前节点的hostname为：${HostName}"
    new_hostname=$1
    hostname ${new_hostname}
    echo "修改当前节点的hostname为：${new_hostname}"
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "完成角色切换"
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"
}
function initEnv(){
    tmpfile="/root/installHadoopCluster/tmp.ini"
    rolefile="/root/installHadoopCluster/role.ini"
    iplistfile="/root/installHadoopCluster/iplist.ini"
    echo "确认当前role.ini路径 : ${rolefile}"
    echo "确认当前iplist.ini路径 : ${iplistfile}"
    normalizdinifile ${rolefile} ${iplistfile} ${tmpfile}
    initIPListDict ${iplistfile}
    initRoleListDict ${rolefile}
    initNodeRoleDict ${rolefile} ${tmpfile}
    echo "初始化IPListDict字典内容："
    for k in "${!IPListDict[@]}"
    do
        echo $k '--' ${IPListDict[$k]}
    done
    echo "初始化RoleListDict字典内容："
    for k in "${!RoleListDict[@]}"
    do
        echo $k '--' ${RoleListDict[$k]}
    done
    echo "初始化NodeRoleDict字典内容："
    for k in "${!NodeRoleDict[@]}"
    do
        echo $k '--' ${NodeRoleDict[$k]}
    done
}
function install(){
    initEnv
    ifcfg_file='/etc/sysconfig/network-scripts/ifcfg-ens18'
    hostsfile='/etc/hosts'
    ntp_config_file="/etc/ntp.conf"
    ssh_user='root'
    sysintfile="/etc/profile"
    kafka_config='/usr/local/hadoop/kafka_2.11-0.9.0.1/config/server.properties'
    mysql_tarfilepath='/usr/local/package/mysql-5.7.6-m16-linux-glibc2.5-x86_64.tar.gz'
    mysql_desfilepath='/usr/local/'
    mysql_filename='mysql-5.7.6-m16-linux-glibc2.5-x86_64'
    mysql_cnfpath='/usr/local/package/my.cnf'
    read -p "输入要设置的IP地址：" IP
    echo "确认当前输入IP : ${IP}"
    changeHostIP ${ifcfg_file} ${IP}
    write2hosts ${iplistfile} ${hostsfile}
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"
    ntp_master_node=${RoleListDict["ntp_master"]}
    ntp_master_ip=${IPListDict["$ntp_master_node"]}
    ntp_master_deploy ${HostName} ${ntp_config_file} ${ntp_master_ip}
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点安装"
        installMySQL ${mysql_tarfilepath} ${mysql_desfilepath} ${mysql_filename} ${mysql_cnfpath}
        echo "执行MySQL节点安装成功"
    else
        echo "不需要执行MySQL节点安装"
    fi
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    is_HDFS_salve=$(substr ${HostRole} "hdfs_slave")
    if [[ ${is_HDFS_master} == "1" || ${is_HDFS_salve} == "1" ]]
    then
        echo "开始执行HDFS安装部署"
        hdfs_master_node=${RoleListDict["hdfs_master"]}
        hdfs_second_master_node=${RoleListDict["hdfs_master"]}
        hdfs_slave_node=${RoleListDict["hdfs_slave"]}
        hdfs_slave_nodes=$(replaceBlank2Comma ${hdfs_slave_node})
        hdfs_node_install ${hdfs_master_node} ${hdfs_second_master_node} ${hdfs_slave_nodes}
        echo "执行HDFS安装部署成功"
    else
        echo "不需要执行HDFS安装部署"
    fi
    is_HBase_master=$(substr ${HostRole} "hbase_master")
    is_HBase_salve=$(substr ${HostRole} "hbase_slave")
    if [[ ${is_HBase_master} == "1" || ${is_HBase_salve} == "1" ]]
    then
        echo "开始执行HBase安装部署"
        zookeeper_node=${RoleListDict["zookeeper"]}
        hbase_master_node=${RoleListDict["hbase_master"]}
        hbase_slave_node=${RoleListDict["hbase_slave"]}
        hbase_nodes=$(replaceBlank2Comma ${hbase_slave_node})
        zookeeper_nodes=$(replaceBlank2Comma ${zookeeper_node})
        hbase_node_install ${hbase_master_node} ${hbase_nodes} ${zookeeper_nodes}
        echo "执行HBase安装部署成功"
    else
        echo "不需要执行HBase安装部署"
    fi
    is_zookeeper=$(substr ${HostRole} "zookeeper")
    if [ ${is_zookeeper} == "1" ]
    then
        echo "开始执行zookeeper安装部署"
        sh zookeeper-install.sh -q ${zookeeper_nodes}
        echo "执行zookeeper安装部署成功"
    else
        echo "不需要执行zookeeper安装部署"
    fi
    is_Kafka=$(substr ${HostRole} "kafka")
    if [ ${is_Kafka} == "1" ]
    then
        echo "开始执行kafka安装部署"
        kafka_nodes=${RoleListDict["kafka"]}
        installKafka "${zookeeper_node}" ${kafka_config}
        echo "执行kafka安装部署成功"
    else
        echo "不需要执行kafka安装部署"
    fi
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES安装部署"
        installES ${HostName}
        echo "执行ES安装部署成功"
    else
        echo "不需要执行ES安装部署"
    fi
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat安装部署"
        installfilebeat
        updatefilebeat ${HostName} ${IP}
        echo "执行filebeat安装部署成功"
    else
        echo "不需要执行filebeat安装部署"
    fi
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash安装部署"
        installlogstash
        updatelogstash ${IP}
        echo "执行logstash安装部署成功"
    else
        echo "不需要执行logstash安装部署"
    fi
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana安装部署"
        installkibana
        updatekibana ${IP}
        echo "执行kibana安装部署成功"
    else
        echo "不需要执行kibana安装部署"
    fi
    echo "当前节点安装部署结束"
}
function startup(){
    initEnv
    ssh_user='root'
    kafka_root_path='/usr/local/hadoop/kafka_2.11-0.9.0.1'
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"
    host=${IPListDict[*]}
    delete=(${HostIP})
    host=( "${host[@]/$delete}" )
    echo "当前需要测试SSH的节点有：${host}"
    ssh2nodes ${ssh_user} "${host}"
    ssh_result=`echo $?`
    if [ ${ssh_result} == "1" ]
    then
        echo "SSH链接测试成功，开始执行后续组件启动"
    else
        echo "SSH链接测试失败，请检查是否有节点关闭或者无法连接，然后重新执行服务启动"
    fi
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点服务启动"
        startMySQL
        echo "执行MySQL节点服务启动成功"
    else
        echo "不需要执行MySQL节点服务启动"
    fi
    zookeeper_node=${RoleListDict["zookeeper"]}
    echo "开始启动zookeeper集群各个节点"
    startup_Cluster_zookeeper "${zookeeper_node}"
    iszookeeperclusterup=`echo $?`
    if [ ${iszookeeperclusterup} == "0" ]
    then
        echo "执行zookeeper集群启动成功"
    else
        echo "执行zookeeper集群启动失败，请检查日志，重新启动"
    fi
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点服务启动"
        hdfs_master_startup
        echo "执行HDFS主节点服务启动成功"
    else
        echo "不需要执行HDFS主节点服务启动"
    fi
    is_HBase_master=$(substr ${HostRole} "hbase_master")
    if [ ${is_HBase_master} == "1" ]
    then
        echo "开始执行HBase主节点服务启动"
        hbase_master_startup
        echo "执行HBase主节点服务启动成功"
    else
        echo "不需要执行HBase主节点服务启动"
    fi
    is_Kafka=$(substr ${HostRole} "kafka")
    if [ ${is_Kafka} == "1" ]
    then
        echo "开始执行kafka节点服务启动"
        startupKafka ${kafka_root_path}
        echo "执行kafka节点服务启动成功"
    else
        echo "不需要执行kafka节点服务启动"
    fi
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES节点服务启动"
        startES
        echo "执行ES节点服务启动成功"
    else
        echo "不需要执行ES节点服务启动"
    fi
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat节点服务启动"
        startfilebeat
        echo "执行filebeat节点服务启动成功"
    else
        echo "不需要执行filebeat节点服务启动"
    fi
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash节点服务启动"
        startlogstash
        echo "执行logstash节点服务启动成功"
    else
        echo "不需要执行logstash节点服务启动"
    fi
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana节点服务启动"
        startkibana
        echo "执行kibana节点服务启动成功"
    else
        echo "不需要执行kibana节点服务启动"
    fi
    echo "当前节点服务启动结束"
}
function stop(){
    initEnv
    kafka_root_path='/usr/local/hadoop/kafka_2.11-0.9.0.1'
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"
    is_Kafka=$(substr ${HostRole} "kafka")
    if [ ${is_Kafka} == "1" ]
    then
        echo "开始执行kafka节点关闭"
        echo "执行kafka节点关闭成功"
    else
        echo "不需要执行kafka节点启动"
    fi
    is_HBase_master=$(substr ${HostRole} "hbase_master")
    if [ ${is_HBase_master} == "1" ]
    then
        echo "开始执行HBase主节点关闭"
        hbase_master_stop
        echo "执行HBase主节点关闭成功"
    else
        echo "不需要执行HBase主节点关闭"
    fi
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点关闭"
        hdfs_master_stop
        echo "执行HDFS主节点关闭成功"
    else
        echo "不需要执行HDFS主节点关闭"
    fi
    zookeeper_node=${RoleListDict["zookeeper"]}
    echo "开始启动zookeeper集群各个节点"
    stop_Cluster_zookeeper "${zookeeper_node}"
    iszookeeperclusterdown=`echo $?`
    if [ ${iszookeeperclusterdown} == "0" ]
    then
        echo "执行zookeeper集群停止成功"
    else
        echo "执行zookeeper集群停止失败，请检查日志，重新停止"
    fi
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点服务关闭"
        stopMySQL
        echo "执行MySQL节点服务关闭成功"
    else
        echo "不需要执行MySQL节点服务关闭"
    fi
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES节点服务关闭"
        stopES
        echo "执行ES节点服务关闭成功"
    else
        echo "不需要执行ES节点服务关闭"
    fi
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat节点服务关闭"
        stopfilebeat
        echo "执行filebeat节点服务关闭成功"
    else
        echo "不需要执行filebeat节点服务关闭"
    fi
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash节点服务关闭"
        stoplogstash
        echo "执行logstash节点服务关闭成功"
    else
        echo "不需要执行logstash节点服务关闭"
    fi
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana节点服务关闭"
        stopkibana
        echo "执行kibana节点服务关闭成功"
    else
        echo "不需要执行kibana节点服务关闭"
    fi
    echo "当前节点服务关闭结束"
}
function entery(){
    echo "执行安装，请输入：install "
    echo "执行服务启动，请输入：startup "
    echo "执行服务停止，请输入：stop"
    read -p "输入要进行的选项：" option
    if [ ${option} == "install" ]
    then
        echo "开始执行安装程序："
        install
    elif [ ${option} == "startup" ]
    then
        echo "开始执行服务启动程序："
        startup
    elif [ ${option} == "stop" ]
    then
        echo "开始执行服务停止程序："
        stop
    fi
}
entery
