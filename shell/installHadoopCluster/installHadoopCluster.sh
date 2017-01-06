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


################ Stage 1: 基础工具定义 ################

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


# 子字符串包含判断：[Bash Shell怎么判断字符串的包含关系](https://segmentfault.com/q/1010000000114932)
function substr(){
    STRING_A=$1
    STRING_B=$2

    if [[ ${STRING_A/${STRING_B}//} == $STRING_A ]]
    then
        ## is not substring.
        echo 0
    else
        ## is substring.
        echo 1
    fi
}

# 替换变量中的空格符为逗号分隔
function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}


################ Stage 2: 系统基本组件安装 ################
# 安装JDK
function installJDK(){
    tarfilepath=$1
    tarfoldername=$2
    destfilepath=$3
    destfolderename=$4
    
    # 解压到指定文件夹：
    eval "tar -zxf ${tarfilepath} -C ${destfilepath}"
    cd ${destfilepath}
    mv ${tarfoldername}/ ${destfolderename}

    # 添加服务启动脚本
    sysintfile="/etc/profile"
    echo "export JAVA_HOME=${destfilepath}/${destfolderename}" >> ${sysintfile}
    echo "export PATH=$PATH:$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH" >> ${sysintfile}
    echo "export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib" >> ${sysintfile}

    # 刷新环境变量
    eval "source ${sysintfile}"
}

# 安装MySQL
function installMySQL(){
    
    tarfilepath=$1
    desfilepath=$2
    filename=$3
    # 已经有的设置文件，cnfpath='/usr/local/my.cnf'
    cnfpath=$4

    # 解压tar包到指定文件夹
    tar -zxf ${tarfilepath} -C ${desfilepath}
    cd ${desfilepath}
    mv ${filename}  ./mysql
    
    # 添加服务初始化表
    groupadd mysql
    useradd -r -g mysql mysql
    chown -R mysql:mysql ./mysql
    ./mysql/bin/mysqld --initialize-insecure --user=mysql  
	
    #需要更新my.cnf文件
    cp  ${cnfpath} /etc/my.cnf 
}

# 启动MySQL服务
function startMySQL(){
     cd /usr/local/mysql/support-files/
	 ./mysql.server start
}

# 关闭MySQL服务
function stopMySQL(){
     cd /usr/local/mysql/support-files/
	 ./mysql.server stop
}

# 允许远程登陆用户添加：
function addRomoteLoginUser(){
    cd /usr/local/mysql/bin/

    mysqllogin="root"
    mysqlpass="" 
    newusername="iscas"
    newuserpass="12345"
    sql_createuser="CREATE USER '${newusername}' IDENTIFIED BY '${newuserpass}';";
    sql_grant1="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'%';";
    sql_grant2="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'localhost';";
    sql_add="${sql_createuser}${sql_grant1}${sql_grant2}";
    ./mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}

# 删除用户：
function deleteuser(){
    cd /usr/local/mysql/bin/
    delusername="iscas"
    mysqllogin="root" 
    mysqlpass="" 

    sql_del1="DROP user '${delusername}'@'%';";
    sql_del2="DROP user '${delusername}'@localhost;";
    sql_add="${sql_del1}";
   ./mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}

# root用户登录：
function rootUserLogin(){
    cd /usr/local/mysql/bin/
    mysqllogin="root"
    mysqlpass=""
   ./mysql --user=$mysqllogin --password=$mysqlpass ;
   #修改root密码
   #newpass="12345"
   #sql_db="use mysql;";
   #sql_change="update user set authentication_string =password('${newpass}') where user='${mysqllogin}'; ";
   #sql_flush="flush privileges;";
   #sql_add="${sql_db}${sql_change}${sql_flush}";
   #echo $sql_add;
   #./mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}

# 使用SQL脚本创建表：
function createtable(){
    cd /usr/local/mysql/bin/
    mysqllogin="iscas" 
    mysqlpass="12345" 

    #locahost登录
    createdb="test";   
    location="/usr/local/componetmetadata.sql";
    ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
    sql_db="Create Database if Not Exists ${createdb} character set utf8 ;";
    select_db="use ${createdb};";
    sql_loc="source ${location};";
    sql_add="${sql_db}${select_db}${sql_loc}";
   ./mysql --user=$mysqllogin --password=$mysqlpass  -h$ipaddr --execute="$sql_add";
}

# 本机登陆：
function localhostlogin(){
    cd /usr/local/mysql/bin/
    ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
    #echo $ipaddr;
    mysqllogin="iscas"
    mysqlpass="12345"
   ./mysql --user=$mysqllogin --password=$mysqlpass -h$ipaddr;
}

# 安装ELK
function installELK(){
    desfilepath='/usr/local/'
    #elasticsearch安装位置
    es_tarpath='/usr/local/elasticsearch-1.6.0.tar.gz'
    filename_es='elasticsearch-1.6.0'
    #logstash安装位置
    log_tarpath='/usr/local/logstash-1.5.6.tar.gz'
    filename_log='logstash-1.5.6'
    #kibana安装位置
    kb_tarpath='/usr/local/kibana-4.0.3-linux-x64.tar.gz'
    filename_kb='kibana-4.0.3-linux-x64'
    #filebeat安装位置
    fb_tarpath='/usr/local/filebeat-1.0.0-x86_64.tar.gz'
    filename_fb='filebeat-1.0.0-x86_64'


    # 解压tar包到指定文件夹
    tar -zxf ${es_tarpath} -C ${desfilepath}  
	#es的读写权限
    cd ${desfilepath}
	chmod -R 775 ./elasticsearch-1.6.0	
	
    # 解压tar包到指定文件夹
    tar -zxf ${log_tarpath} -C ${desfilepath}
	
    # 解压tar包到指定文件夹
    tar -zxf ${kb_tarpath} -C ${desfilepath}  

    # 解压tar包到指定文件夹
    tar -zvxf ${fb_tarpath} -C ${desfilepath}  
}

# 更新ELK配置文件：
function updateYML(){
   #更新es node name
   FILE=/usr/local/elasticsearch-1.6.0/config/elasticsearch.yml
   KEY='node.name: "node_es1"'
   INSTEAD='node.name: "node_es2"'
   sed -i  's/'"$KEY"'/'"$INSTEAD"'/g'  $FILE
   
   #更新logstash所需conf
   conf1path='/usr/local/beatsfinal.conf'
   cp ${conf1path}  /usr/local/logstash-1.5.6/
   cd /usr/local/logstash-1.5.6/
   FILE=./beatsfinal.conf
   ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
   KEY="0.0.0.0"
   sed -i  's/'"$KEY"'/'"$ipaddr"'/g'  $FILE

   #更新kibana所需conf
   cd /usr/local/kibana-4.0.3-linux-x64/config/
   FILE=./kibana.yml
   ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
   KEY1="0.0.0.0"
   KEY2="localhost:"
   sed -i  's/'"$KEY1"'/'"$ipaddr"'/g'  $FILE
   sed -i  's/'"$KEY2"'/'"$ipaddr":'/g'  $FILE

   
   #更新filebeat所需 yml
   conf2path='/usr/local/filebeat.yml'
   cp ${conf2path}  /usr/local/filebeat-1.0.0-x86_64/
     cd /usr/local/filebeat-1.0.0-x86_64/
   FILE=./filebeat.yml
   KEY='#name:'
   INSTEAD='name: beats1'
   sed -i  's/'"$KEY"'/'"$INSTEAD"'/g'  $FILE
}

# 启动ES
function startES(){
    /usr/local/elasticsearch-1.6.0/bin/elasticsearch  -d
}

# 启动logstash
function startlogstash(){
   cd /usr/local/logstash-1.5.6/
   ./bin/logstash -f ./beatsfinal.conf &
}

# 启动kibana
function startkibana(){
    cd /usr/local//kibana-4.0.3-linux-x64/
    ./bin/kibana &
}

# 启动filebeat
function startfilebeat(){
  cd /usr/local/filebeat-1.0.0-x86_64/
  ./filebeat -e -c ./filebeat.yml   &
}

# 关闭ES
function stopES(){
    ps -ef | grep elasticsearch | awk '{print $2}'|xargs kill 
}

# 关闭logstash
function stoplogstash(){
    ps -ef | grep logstash | awk '{print $2}'|xargs kill
}

# 关闭kibana
function stopkibana(){
    ps -ef | grep kibana | awk '{print $2}'|xargs kill
}

# 关闭filebeat
function stopfilebeat(){
   ps -ef | grep filebeat | awk '{print $2}'|xargs kill
}


################ Stage 3: 配置文件解析和获取参数 ################

# 初始化ini文件：去除\r\n导致的识别问题
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

# 初始化IPList字典
function initIPListDict(){
    # read iplist.ini
    file=$1
    # get node ip
    source ${file}
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10)
}

# 初始化角色字典
function initRoleListDict(){
    # read iplist.ini
    file=$1
    source ${file}
    RoleListDict=([ntp_master]=$ntp_master [hdfs_master]=$hdfs_master [hdfs_slave]=$hdfs_slave [zookeeper]=$zookeeper [hbase_master]=$hbase_master [hbase_slave]=$hbase_slave [kafka]=$kafka)
}


# 初始化节点角色字典：
function initNodeRoleDict(){
    file=$1
    tmpfile=$2
    echo `cat $file |awk -F'=' '{split($2,arr,",");for(i=1;i<=length(arr);i++)z[arr[i]]=z[arr[i]]?z[arr[i]]","$1:$1}END{for(y in z)print y"="z[y]}'` >> $tmpfile
    eval "source $tmpfile"
    NodeRoleDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10)
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


################ Stage 4: 系统环境准备 ################

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

################ Stage 5: 根据当前节点的角色进行设置 ################
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
    currentHostRole=${NodeRoleDict["$currentHostName"]}
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
    currenthostname=$1
    ntp_config_file=$2
    ntp_master_ip=$3

    # 获取当前节点的角色
    currentHostRole=$(getCurrentHOSTRole ${currenthostname})

    # 然后判断是否包含
    isContain=$(substr ${currentHostRole} "ntp_master")
    if [ ${isContain} == "1" ]
    then
        ## NTP主节点：
        echo "ntp master detected"
        ntp_master_config ${ntp_config_file}
        return 1
    else
        ## NTP子节点：
        echo "ntp salve detected"
        ntp_slaver_config ${ntp_config_file} ${ntp_master_ip}
        return 0
    fi
}

# SSH到子节点，执行命令退出
function ssh2nodes(){
    username=$1
    iplist=$2
    script="pwd; ls"

    # 默认全部连接成功
    val=1
    for currentip in ${iplist}
    do
        echo "current ssh to : ${currentip}"
        local RESULTS
        RESULTS=$(ssh -l ${username} ${currentip} "${script}")
        tmp=`echo $?`
        # 如果ssh执行失败，返回255
        if [ "$tmp" == 255 ]
        then
            val=0
            break
        fi
    done
    return "$val"
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


################ Stage 6: 测试环境设置 ################
# 测试脚本执行前的环境设置
function testInit(){
    # 测试前清空缓存：
    rm -rf /home/wentao/shell/dest/*

    # 重新获取测试文件：
    cp /etc/sysconfig/network-scripts/ifcfg-ens33 /home/wentao/shell/dest/
    cp /etc/hosts /home/wentao/shell/dest/
    cp /etc/ntp.conf /home/wentao/shell/dest/
    cp /etc/profile /home/wentao/shell/dest/
    cp /home/wentao/shell/tmp/server.properties.bak /home/wentao/shell/dest/server.properties

    # 修改主机名，完成角色替换
    HostName=$(getCurrentHostName)
    echo "当前节点的hostname为：${HostName}"
    new_hostname=$1
    hostname ${new_hostname}
    echo "修改当前节点的hostname为：${new_hostname}"

    # 获取当前角色
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "完成角色切换"
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"
}


################ Stage 7: 组织完成安装部署的完整调用流程 ################
function run(){

    ################# 脚本中使用的静态路径 #################
    # 设置网卡配置文件路径：
    ifcfg_file='/home/wentao/shell/dest/ifcfg-ens33'
    # 设置当前hosts文件路径：
    hostsfile='/home/wentao/shell/dest/hosts'
    # 设置ntp服务的配置文件：
    ntp_config_file="/home/wentao/shell/dest/ntp.conf"
    # SSH户名设置：
    ssh_user='root'
    # 设置jdk安装常量：
    tarfilepath="/home/wentao/shell/package/jdk-8u112-linux-x64.tar.gz"
    tarfoldername="jdk1.8.0_112"
    destfilepath="/home/wentao/shell/dest/"
    destfolderename="JDK"
    sysintfile="/home/wentao/shell/dest/profile"
    # 初始化NodeRoleDict需要的临时文件路径：
    tmpfile="/home/wentao/shell/dest/tmp.ini"
    # 设置kafka的配置文件路径：
    kafka_config='/home/wentao/shell/dest/server.properties'
    # role.ini文件路径：
    rolefile="/home/wentao/shell/installHadoopCluster/role.ini"
    # iplist.ini文件路径：
    iplistfile="/home/wentao/shell/installHadoopCluster/iplist.ini"
    # 设置MySQL安装常量：


    ################# 获取用户输入 #################
    # 输入当前节点中role.ini文件所在的位置：
    #read -p "输入role.ini文件所在的位置：" rolefile
    # 输入当前节点中iplist.ini文件所在的位置：
    #read -p "输入iplist.ini文件所在的位置：" iplistfile
    # 输入当前节点要设置的IP地址：
    read -p "输入要设置的IP地址：" IP


    ################# 确认用户输入 #################
    echo "确认当前输入IP : ${IP}"
    echo "确认当前role.ini路径 : ${rolefile}"
    echo "确认当前iplist.ini路径 : ${iplistfile}"


    ################# 初始化全局设置 #################
    # 规范输入文件：
    normalizdinifile ${iplistfile} ${rolefile} ${tmpfile}
    # 初始化IPListDict：
    initIPListDict ${iplistfile}
    # 初始化RoleListDict:
    initRoleListDict ${rolefile}
    # 初始化NodeRoleDict：
    initNodeRoleDict ${rolefile} ${tmpfile}

    # 查看字典初始化结果：传入关联数组作为参数
    echo "查看IPListDict字典："
    for k in "${!IPListDict[@]}"
    do
        echo $k '--' ${IPListDict[$k]}
    done
    #getDictContent IPListDict[@]
    echo "查看RoleListDict字典："
    for k in "${!RoleListDict[@]}"
    do
        echo $k '--' ${RoleListDict[$k]}
    done
    #getDictContent RoleListDict[@]
    echo "查看NodeRoleDict字典："
    for k in "${!NodeRoleDict[@]}"
    do
        echo $k '--' ${NodeRoleDict[$k]}
    done
    #getDictContent NodeRoleDict[@]


    ################# 基本环境安装 #################
    #installJDK ${tarfilepath} ${tarfoldername} ${destfilepath} ${destfolderename} ${sysintfile}


    ################# 开始进行调用 #################
    # （1）根据当前用户的输入，设置本机IP
    changeHostIP ${ifcfg_file} ${IP}
    #changeHostIP '/etc/sysconfig/network-scripts/ifcfg-ens18' '192.168.1.100'

    # （2）根据用户指定的iplist.ini文件，设置hosts
    write2hosts ${iplistfile} ${hostsfile}

    # （3）获取当前角色:
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"

    # （4）根据当前角色设置NTP服务：
    # 根据role.ini获取ntp服务器的ip
    ntp_master_node=${RoleListDict["ntp_master"]}
    ntp_master_ip=${IPListDict["$ntp_master_node"]}
    ntp_master_deploy ${ntp_config_file} ${ntp_master_ip}

    # （5）根据当前角色进行ssh测试：在安装中不需要进行SSH测试，在启动服务的时候进行测试
    # 获取IPListDict的所有IP值进行连接测试
    host=${IPListDict[*]}
    # 需要除去自己当前的ip，否则无法连接成功：[Remove element from array shell](http://stackoverflow.com/questions/16860877/remove-element-from-array-shell)
    delete=(${IP})
    host=( "${host[@]/$delete}" )
    # 开始测试，测试正常将返回1，如果有一个节点无法ssh通就表示失败
    ssh2nodes ${ssh_user} ${host}
    ssh_result=`echo $?`
    # if判断的时候，注意格式，需要留出空格才行
    if [ ${ssh_result} == "1" ]
    then
        echo "SSH链接测试成功"
    else
        echo "SSH链接测试失败"
    fi

    # （6）根据当前角色安装HDFS：
    isHDFS_master=$(substr ${HostRole} "hdfs_master")
    isHDFS_salve=$(substr ${HostRole} "hdfs_slave")
    echo "is hdfs master:${isHDFS_master}"
    echo "is hdfs slave:${isHDFS_salve}"
    if [[ ${isHDFS_master} == "1" || ${isHDFS_salve} == "1" ]]
    then
        echo "执行HDFS安装部署"
        hdfs_master_node=${RoleListDict["hdfs_master"]}
        hdfs_second_master_node=""
        hdfs_slave_node=${RoleListDict["hdfs_slave"]}
        hdfs_slave_nodes=$(replaceBlank2Comma ${hdfs_slave_node})
        sh hadoop-install.sh -m ${hdfs_master_node} -n ${hdfs_second_master_node} -s ${hdfs_slave_nodes}
    else
        echo "不需要执行HDFS安装部署"
    fi

    # （7）根据当前角色安装HBase：
    isHBase_master=$(substr ${HostRole} "hbase_master")
    isHBase_salve=$(substr ${HostRole} "hbase_slave")
    if [[ ${isHBase_master} == "1" || ${isHBase_salve} == "1" ]]
    then
        echo "执行HBase安装部署"
        zookeeper_node=${RoleListDict["zookeeper"]}
        hbase_master_node=${RoleListDict["hbase_master"]}
        hbase_slave_node=${RoleListDict["hbase_slave"]}
        hbase_nodes=$(replaceBlank2Comma ${hbase_slave_node})
        zookeeper_nodes=$(replaceBlank2Comma ${zookeeper_node})
        sh hbase-install.sh -m ${hbase_master_node} -s ${hbase_nodes} -z ${zookeeper_nodes}
    else
        echo "不需要执行HBase安装部署"
    fi

    # （8）根据当前角色安装zookeeper：
    is_zookeeper=$(substr ${HostRole} "zookeeper")
    if [ is_zookeeper == "1" ]
    then
        echo "执行zookeeper安装部署"
        sh zookeeper-install.sh -q ${zookeeper_nodes}
    else
        echo "执行zookeeper安装部署"
    fi
    
    # （9）安装kafka：
    isKafka=$(substr ${HostRole} "kafka")
    if [ ${isKafka} == "1" ]
    then
        echo "执行kafka安装部署"
        kafka_nodes=${RoleListDict["kafka"]}
        setup_kafka "${zookeeper_node}" ${kafka_config}
    else
        echo "不需要执行kafka安装部署"
    fi
    

    ################# 完成自动化部署 #################
    echo "当前节点安装部署结束"
}


################ Stage 8: 部署 ################
testInit "node1"
run
