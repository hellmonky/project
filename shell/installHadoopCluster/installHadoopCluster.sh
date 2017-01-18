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
# 定义全局Role对应的IP字典
declare -A RoleIPDict


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
        ## A do not contains B
        echo 0
    else
        ## A contains B
        echo 1
    fi
}

# 替换变量中的空格分隔为逗号
function replaceBlank2Comma(){
    context="$@"
    b=${context// /,}
    echo ${b}
}

# 替换变量中的逗号分隔符为空格
function replaceComma2Blank(){
    context="$@"
    echo "$context"|sed "s/,/ /g"
}

# 解压缩tar包到指定文件路径
function releaseTarPackage(){
    tarfilepath=$1
    desfilepath=$2
    tar -zxf ${tarfilepath} -C ${desfilepath}
}

################ Stage 2: 系统基本组件安装 ################
#------------------- 安装JDK ------------------
function installJDK(){
    tarfilepath=$1
    tarfoldername=$2
    destfilepath=$3
    destfolderename=$4
    
    # 解压到指定文件夹：
    eval "tar -zxf ${tarfilepath} -C ${destfilepath}"
    mv ${destfilepath}/${tarfoldername}/ ${destfilepath}/${destfolderename}

    # 添加服务启动脚本
    sysintfile="/etc/profile"
    echo "export JAVA_HOME=${destfilepath}/${destfolderename}" >> ${sysintfile}
    echo "export PATH=$PATH:$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH" >> ${sysintfile}
    echo "export CLASSPATH=.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib" >> ${sysintfile}

    # 刷新环境变量
    eval "source ${sysintfile}"
}

#------------------- 安装MySQL ------------------
function installMySQL(){
    tarfilepath=$1
    desfilepath=$2
    filename=$3
    # 已经有的设置文件，cnfpath='/usr/local/my.cnf'
    cnfpath=$4

    # 解压tar包到指定文件夹
    tar -zxf ${tarfilepath} -C ${desfilepath}
    mv ${desfilepath}/${filename} ${desfilepath}/mysql

    # 不要使用cd切换，因为会造成外部执行路径地址改变！
    #cd ${desfilepath}
    #mv ${filename}  ./mysql
    
    # 添加服务初始化表
    groupadd mysql
    useradd -r -g mysql mysql
    chown -R mysql:mysql ${desfilepath}/mysql
    ${desfilepath}/mysql/bin/mysqld --initialize-insecure --user=mysql  
	
    #需要更新my.cnf文件
    cp  ${cnfpath} /etc/my.cnf 
}

# 启动MySQL服务
function startMySQL(){
	 /usr/local/mysql/support-files/mysql.server start
}

# 关闭MySQL服务
function stopMySQL(){
	 /usr/local/mysql/support-files/mysql.server stop
}

# 获取当前节点上MySQL的PID，如果是0就没有启动，否则就是对应的PID值：
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

# 允许远程登陆用户添加：
function addRomoteLoginUser(){
    mysqllogin="root"
    mysqlpass="" 
    newusername="iscas"
    newuserpass="123456"
    sql_createuser="CREATE USER '${newusername}' IDENTIFIED BY '${newuserpass}';";
    sql_grant1="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'%';";
    sql_grant2="GRANT ALL PRIVILEGES ON *.* TO '${newusername}'@'localhost';";
    sql_add="${sql_createuser}${sql_grant1}${sql_grant2}";
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}

# 删除用户：
function deleteuser(){
    delusername="iscas"
    mysqllogin="root" 
    mysqlpass="" 

    sql_del1="DROP user '${delusername}'@'%';";
    sql_del2="DROP user '${delusername}'@localhost;";
    sql_add="${sql_del1}";
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass --execute="$sql_add";
}

# root用户登录：
function rootUserLogin(){
    mysqllogin="root"
    mysqlpass=""
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass ;
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
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass  -h$ipaddr --execute="$sql_add";
}

# 本机登陆：
function localhostlogin(){
    ipaddr=`ip route show | sed -n '2'p|awk '{print $9}'`
    #echo $ipaddr;
    mysqllogin="iscas"
    mysqlpass="12345"
    /usr/local/mysql/bin/mysql --user=$mysqllogin --password=$mysqlpass -h$ipaddr;
}

#------------------- ELK相关的IP获取 ------------------
function GetELKIP(){
   name=$1
   OLD_IFS="$IFS"
   IFS=","
   arr=(${RoleIPDict[$name]})
   IFS="$OLD_IFS"
   ELKIP=""
   for element in ${arr[@]}   
   do  
       if [ $name == "es" ];then
          ELKIP=$ELKIP"\""$element"\""","
       else
          ELKIP=$ELKIP$element","
       fi
   done
   ELKIP=${ELKIP%?}
   echo $ELKIP
 }

#------------------- 安装ES ------------------
# 解压缩ES
function releaseES(){
    #elasticsearch安装位置
    es_tarpath='/usr/local/package/elasticsearch-1.6.0.tar.gz'
    desfilepath='/usr/local/'   
    foldername='elasticsearch-1.6.0'

    releaseTarPackage ${es_tarpath} ${desfilepath}
    #es的读写权限
    chmod -R 775 ${desfilepath}/${foldername}
}

# 设置ES配置文件为当前节点
function updateES(){
   #更新es node name
   nodename=$1

   FILE=/usr/local/elasticsearch-1.6.0/config/elasticsearch.yml
   KEY='node.name: "node_es1"'
   INSTEAD='node.name: '${nodename}''
   sed -i  's/'"$KEY"'/'"$INSTEAD"'/g'  $FILE
}

# 在当前节点安装ES
function installES(){
    currentHostname=$1
    releaseES
    updateES ${currentHostname}
}

# 在当前节点启动ES
function startES(){  
    /usr/local/elasticsearch-1.6.0/bin/elasticsearch  -d
}

# 停止当前节点的ES
function stopES(){
    ps -ef | grep elasticsearch | grep -v grep | awk '{print $2}'| xargs kill
}

#------------------- 安装FileBeat ------------------
function installfilebeat(){
    #filebeat安装位置
    fb_tarpath='/usr/local/package/filebeat-1.0.0-x86_64.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${fb_tarpath} -C ${desfilepath}
}
function updatefilebeat(){
    currentHostname=$1
    currentIP=$2

     #更新filebeat所需 yml
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

#------------------- 安装logstah ------------------
function installlogstash(){
    #logstash安装位置
    log_tarpath='/usr/local/package/logstash-1.5.6.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${log_tarpath} -C ${desfilepath}
}
function updatelogstash(){
    # 当前节点的IP
    ipaddr=$1

    #更新logstash所需conf
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

#------------------- 安装kibana ------------------
function installkibana(){
    #kibana安装位置
    kb_tarpath='/usr/local/package/kibana-4.0.3-linux-x64.tar.gz'
    desfilepath='/usr/local/'
    tar -zxf ${kb_tarpath} -C ${desfilepath}  
}

# 设置kibana配置文件中为本机IP
function updatekibana(){
    # 当前节点的IP
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

#------------------- 安装kafka ------------------
# 安装kafka，并且完成设置，可能需要修改kafka下的关闭脚本。[kafka集群脚本](http://www.voidcn.com/blog/u012373815/article/p-6357559.html)
function installKafka(){
    # 获取对应的主机名
    ZOOKEEPER_NODES=$1
    # 需要将逗号替换为空格
    ZOOKEEPER_NODES=$(replaceComma2Blank ${ZOOKEEPER_NODES})
    content=""
    for node in ${ZOOKEEPER_NODES} ; do
        content=${content}${node}":2181,"
    done
    length=`expr ${#content} - 1`
    content=${content:0:$length}
    #echo "检查kafka参数："
    #echo ${content}
    # 修改文件命令，设置breaker的id为当前机器的hostname
    KAFKA_CONFIG=$2
    #sed -i "s/^zookeeper.connect=/zookeeper.connect=${content}/g" ${KAFKA_CONFIG}
    sed -i "/zookeeper.connect=localhost:2181/{s/.*/zookeeper.connect="${content}"/}" ${KAFKA_CONFIG}
}

# 启动kafka：需要在每一个kafka节点上进行启动，[kafka 集群--3个broker 3个zookeeper创建实战](http://www.cnblogs.com/davidwang456/p/4238536.html)
function startupKafka(){
    kafka_root_path=$1
    # 需要在后台启动kafka，并且将返回信息写入空或者写入到启动日志中
    ${kafka_root_path}/bin/kafka-server-start.sh ${kafka_root_path}/config/server.properties 1>/dev/null 2>&1 &
}

# 关闭kafka：
function stopKafka(){
    kafka_root_path=$1
    #echo "正在关闭当前节点上的kafka服务"
    ${kafka_root_path}/bin/kafka-server-stop.sh
    #echo "当前节点kafka服务关闭"
    return 0
}

# 启动集群中所有的kafka节点:：
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

# 关闭集群中所有的kafka节点：
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

#------------------- 安装zookeeper ------------------
# 在当前节点执行zookeeper安装：
function install_node_zookeeper(){
    zookeeper_nodes_List=$1
    sh zookeeper-install.sh -q ${zookeeper_nodes_List}
}

# 启动集群中所有的zookeeper节点：
function startup_Cluster_zookeeper(){
    # 获取zookeeper的节点列表，传递参数中包含逗号分隔：
    zookeeper_nodes_List=$1
    # 需要将逗号替换为空格：
    zookeeper_nodes_List=$(replaceComma2Blank ${zookeeper_nodes_List})
    # 设置zookeeper的基本参数：
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

# 关闭集群中所有的zookeeper节点：
function stop_Cluster_zookeeper(){
    # 获取zookeeper的节点列表，传递参数中包含逗号分隔：
    zookeeper_nodes_List=$1
    # 需要将逗号替换为空格：
    zookeeper_nodes_List=$(replaceComma2Blank ${zookeeper_nodes_List})
    # 设置zookeeper的基本参数：
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

# 检查当前节点上zookeeper是否执行：如果执行返回PID，否则返回0
function check_zookeeper(){
    zookeeper_pid=`jps | grep 'QuorumPeerMain' | cut -d ' ' -f 1`
    if [ ! "${zookeeper_pid}" ]; then
        echo 0
    else
        echo ${zookeeper_pid}
    fi
}

#------------------- 安装HBase ------------------
# Hbase子节点安装：需要对每一个Hbase节点进行安装
function hbase_node_install(){
    hbase_master_node=$1
    hbase_nodes=$2
    zookeeper_nodes=$3
    sh hbase-install.sh -m ${hbase_master_node} -s ${hbase_nodes} -z ${zookeeper_nodes}
}


# HBase主节点服务启动，需要将当前所有子节点都部署完毕才能正确执行
function hbase_master_startup(){
    /usr/local/hadoop/hbase-1.2.0/bin/start-hbase.sh
}

# HBase主节点服务关闭，只需要对主节点关闭即可
function hbase_master_stop(){
    /usr/local/hadoop/hbase-1.2.0/bin/stop-hbase.sh
}

# 检查当前节点上HBase是否启动，如果启动返回PID，否则返回0：
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

#------------------- 安装HDFS ------------------
# HDFS子节点安装：需要对每一个HDFS节点进行安装
function hdfs_node_install(){
    hdfs_master_node=$1
    hdfs_second_master_node=$2
    hdfs_slave_nodes=$3
    hdfs_replication_number=$4
    sh hadoop-install.sh -m ${hdfs_master_node} -n ${hdfs_second_master_node} -s ${hdfs_slave_nodes} -r ${hdfs_replication_number}
}

# 需要在HDFS的主节点上执行格式化，其他节点不需要：
function hdfs_master_format(){
    hadoop namenode -format
}

# 需要在HDFS启动前进行初始化设置
function hdfs_master_prepare(){
    # 关闭safe mode：
    hdfs dfsadmin -safemode leave
    # 创建一个HDFS根目录，作为当前项目的起始目录：
    hadoop fs -mkdir /datasong
    # 更改HDFS文件夹的权限
    hadoop fs -chmod 777 /datasong

    # 总体流程描述：
    # 出现问题：[connection-refused](http://stackoverflow.com/questions/28661285/hadoop-cluster-setup-java-net-connectexception-connection-refused)
    # 然后尝试的解决方式为：
    # 1 需要退出safe mode : hdfs dfsadmin -safemode leave
    # 2 然后更改HDFS文件夹的权限？是否需要：hadoop fs -chmod 777 /datasong
    # 3 然后要SSH开放HDFS访问需要的默认9000的的端口：[](http://stackoverflow.com/questions/28661285/hadoop-cluster-setup-java-net-connectexception-connection-refused)
    # 4 将node添加到ssh的konwn hosts文件中：Permanently added 'node3,192.168.1.152' (ECDSA) to the list of known hosts.
}


# HDFS主节点服务启动，需要将当前所有子节点都部署完毕才能正确执行
function hdfs_master_startup(){
    /usr/local/hadoop/hadoop-2.6.0/sbin/start-dfs.sh
    /usr/local/hadoop/hadoop-2.6.0/sbin/start-yarn.sh
}

# HDFS主节点服务关闭，只需要将主节点关闭即可
function hdfs_master_stop(){
    /usr/local/hadoop/hadoop-2.6.0/sbin/stop-dfs.sh
    /usr/local/hadoop/hadoop-2.6.0/sbin/stop-yarn.sh
}

# 检查当前节点上HDFS是否启动，如果启动返回PID，否则返回0：
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
    eval "source ${file}"
    IPListDict=([node1]=$node1 [node2]=$node2 [node3]=$node3 [node4]=$node4 [node5]=$node5 [node6]=$node6 [node7]=$node7 [node8]=$node8 [node9]=$node9 [node10]=$node10)
}

# 初始化角色字典
function initRoleListDict(){
    # read iplist.ini
    file=$1
    eval "source ${file}"
    RoleListDict=([ntp_master]=$ntp_master [hdfs_master]=$hdfs_master [hdfs_slave]=$hdfs_slave [zookeeper]=$zookeeper [hbase_master]=$hbase_master [hbase_slave]=$hbase_slave [kafka]=$kafka [mysql]=$mysql [es]=${es} [filebeat]=${filebeat} [logstash]=${logstash} [kibana]=${kibana})
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

# 初始化角色包含的IP列表：
function initRoleIPDict(){
   for key in $(echo ${!RoleListDict[*]})
   do
      OLD_IFS="$IFS"
      IFS=","
      arr=(${RoleListDict[$key]})
      IFS="$OLD_IFS"
      roleallip=""
      for element in ${arr[@]}   
        do  
          element=${IPListDict[$element]}
          roleallip=$roleallip$element","
        done
      roleallip=${roleallip%?}
      RoleIPDict[$key]=$roleallip
  done
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
    echo "要修改的网卡配置文件为："
    echo ${eth}

    # 修改输入修改ip地址
    sed -i "13c IPADDR=${IP}" ${eth}
    # 重启网卡
    systemctl restart  network.service

    # 判断是否设置成功
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
        # 如果ssh执行失败，返回255，执行成功返回0
        if [ "$tmp" == 0 ]
        then
            echo "当前节点SSH到 ${currentip} 成功"
        else
            val=0
            break
        fi
    done
    return "$val"
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
# 初始化：
function initEnv(){
    ################# 脚本中使用的静态路径 #################
    # role.ini文件路径：
    rolefile="/home/wentao/shell/installHadoopCluster/role.ini"
    # iplist.ini文件路径：
    iplistfile="/home/wentao/shell/installHadoopCluster/iplist.ini"
    # 初始化NodeRoleDict需要的临时文件路径：
    tmpfile="/home/wentao/shell/dest/tmp.ini"

    # 实际调用参数设置：
    #rolefile="/root/installHadoopCluster/role.ini"
    #iplistfile="/root/installHadoopCluster/iplist.ini"
    #tmpfile="/root/installHadoopCluster/tmp.ini"

    echo "确认当前role.ini路径 : ${rolefile}"
    echo "确认当前iplist.ini路径 : ${iplistfile}"


    ################# 初始化全局设置 #################
    # 规范输入文件：
    normalizdinifile ${rolefile} ${iplistfile} ${tmpfile}
    # 初始化IPListDict：
    initIPListDict ${iplistfile}
    # 初始化RoleListDict:
    initRoleListDict ${rolefile}
    # 初始化NodeRoleDict：
    initNodeRoleDict ${rolefile} ${tmpfile}

    # 查看字典初始化结果：传入关联数组作为参数
    echo "初始化IPListDict字典内容："
    for k in "${!IPListDict[@]}"
    do
        echo $k '--' ${IPListDict[$k]}
    done
    #getDictContent IPListDict[@]
    echo "初始化RoleListDict字典内容："
    for k in "${!RoleListDict[@]}"
    do
        echo $k '--' ${RoleListDict[$k]}
    done
    #getDictContent RoleListDict[@]
    echo "初始化NodeRoleDict字典内容："
    for k in "${!NodeRoleDict[@]}"
    do
        echo $k '--' ${NodeRoleDict[$k]}
    done
}

# 服务安装：
function install(){
    ################# 初始化全局变量 #################
    initEnv

    ################# 安装脚本中使用的静态变量 #################
    # 设置网卡配置文件路径：
    ifcfg_file='/home/wentao/shell/dest/ifcfg-ens33'
    # 设置当前hosts文件路径：
    hostsfile='/home/wentao/shell/dest/hosts'
    # 设置ntp服务的配置文件：
    ntp_config_file="/home/wentao/shell/dest/ntp.conf"
    # 设置jdk安装常量：
    tarfilepath="/home/wentao/shell/package/jdk-8u112-linux-x64.tar.gz"
    tarfoldername="jdk1.8.0_112"
    destfilepath="/home/wentao/shell/dest/"
    destfolderename="JDK"
    sysintfile="/home/wentao/shell/dest/profile"
    # 设置kafka的配置文件路径：
    kafka_config='/home/wentao/shell/dest/server.properties'
    # 设置MySQL安装路径：
    mysql_tarfilepath='/usr/local/package/mysql-5.7.6-m16-linux-glibc2.5-x86_64.tar.gz'
    mysql_desfilepath='/usr/local/'
    mysql_filename='mysql-5.7.6-m16-linux-glibc2.5-x86_64'
    mysql_cnfpath='/usr/local/package/my.cnf'

    # 实际调用参数设置：
    #ifcfg_file='/etc/sysconfig/network-scripts/ifcfg-ens18'
    #hostsfile='/etc/hosts'
    #ntp_config_file="/etc/ntp.conf"
    #tarfilepath="/usr/local/package/jdk-8u112-linux-x64.tar.gz"
    #tarfoldername="jdk1.8.0_112"
    #destfilepath="/usr/local/bin/"
    #destfolderename="JDK"
    #sysintfile="/etc/profile"
    #kafka_config='/usr/local/hadoop/kafka_2.11-0.9.0.1/config/server.properties'


    ################# 获取用户输入 #################
    # 输入当前节点要设置的IP地址：
    read -p "输入要设置的IP地址：" IP
    # 确认用户输入
    echo "确认当前输入IP : ${IP}"

    
    ################# 基本环境安装 #################
    #installJDK ${tarfilepath} ${tarfoldername} ${destfilepath} ${destfolderename} ${sysintfile}


    ################# 开始进行各项服务安装 #################
    # （1）根据当前用户的输入，设置本机IP：
    changeHostIP ${ifcfg_file} ${IP}
    #changeHostIP '/etc/sysconfig/network-scripts/ifcfg-ens18' '192.168.1.100'

    # （2）根据用户指定的iplist.ini文件，设置hosts：
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
    ntp_master_deploy ${HostName} ${ntp_config_file} ${ntp_master_ip}

    # （5）根据当前角色安装mysql：
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点安装"
        installMySQL ${mysql_tarfilepath} ${mysql_desfilepath} ${mysql_filename} ${mysql_cnfpath}
        echo "执行MySQL节点安装成功"
        # 添加远程登陆用户
        echo "开始添加MySQL远程登陆用户：iscas 密码：123456"
        addRomoteLoginUser
        echo "添加MySQL远程登陆用户成功"
    else
        echo "不需要执行MySQL节点安装"
    fi
    

    # （6）根据当前角色安装HDFS：
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    is_HDFS_salve=$(substr ${HostRole} "hdfs_slave")
    if [[ ${is_HDFS_master} == "1" || ${is_HDFS_salve} == "1" ]]
    then
        echo "开始执行HDFS安装部署"
        hdfs_master_node=${RoleListDict["hdfs_master"]}
        # 如果没有第二个主节点，就用当前的主节点代替
        hdfs_second_master_node=${RoleListDict["hdfs_master"]}
        hdfs_slave_node=${RoleListDict["hdfs_slave"]}
        hdfs_slave_nodes=$(replaceBlank2Comma ${hdfs_slave_node})
        # 设置数据备份数为3，这个通过其他参数修改
        hdfs_replication_number=3
        hdfs_node_install ${hdfs_master_node} ${hdfs_second_master_node} ${hdfs_slave_nodes} ${hdfs_replication_number}
        echo "执行HDFS安装部署成功"
    else
        echo "不需要执行HDFS安装部署"
    fi
    # 然后执行主节点的格式化
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点格式化"
        hdfs_master_format
        echo "执行HDFS主节点格式化成功"
    else
        echo "不需要执行HDFS主节点格式化"
    fi
    # 然后执行主节点的初始化设置
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点初始化设置"
        hdfs_master_prepare
        echo "执行HDFS主节点初始化设置成功"
    else
        echo "不需要执行HDFS主节点初始化设置"
    fi


    # （7）根据当前角色安装HBase：
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

    # （8）根据当前角色安装zookeeper：
    is_zookeeper=$(substr ${HostRole} "zookeeper")
    if [ ${is_zookeeper} == "1" ]
    then
        echo "开始执行zookeeper安装部署"
        sh zookeeper-install.sh -q ${zookeeper_nodes}
        echo "执行zookeeper安装部署成功"
    else
        echo "不需要执行zookeeper安装部署"
    fi
    
    # （9）安装kafka：
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

    # （10）根据当前角色判断是否需要安装es
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES安装部署"
        installES ${HostName}
        echo "执行ES安装部署成功"
    else
        echo "不需要执行ES安装部署"
    fi

    # （11）根据当前角色判断是否需要安装filebeat
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat安装部署"
        installfilebeat
        name="logstash"
        RoleIP=`GetELKIP $name`
        updatefilebeat ${HostName} ${RoleIP}
        echo "执行filebeat安装部署成功"
    else
        echo "不需要执行filebeat安装部署"
    fi
    
    # （12）根据当前角色判断是否需要安装logstash
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash安装部署"
        installlogstash
        name="es"
        RoleIP=`GetELKIP $name`
        updatelogstash ${RoleIP}
        echo "执行logstash安装部署成功"
    else
        echo "不需要执行logstash安装部署"
    fi

    # （13）根据当前角色判断是否需要安装kibana
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana安装部署"
        installkibana
        name="es" 
        RoleIP=`GetELKIP $name`
        RoleIP=${RoleIP:1:13}
        updatekibana ${RoleIP}
        echo "执行kibana安装部署成功"
    else
        echo "不需要执行kibana安装部署"
    fi


    ################# 完成自动化部署 #################
    echo "当前节点安装部署结束"
}

# 服务启动：
function startup(){
    ################# 初始化全局变量 #################
    initEnv

    ################# 脚本中使用的静态变量 #################
    # SSH户名设置：
    ssh_user='root'
    # kafka根路径：
    kafka_root_path='/usr/local/hadoop/kafka_2.11-0.9.0.1'

    
    ################# 获取当前角色 #################
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"

    ################# SSH通信测试 #################
    # 根据当前角色进行ssh测试
    # 获取IPListDict的所有IP值进行连接测试
    host=${IPListDict[*]}
    # 需要除去自己当前的ip，否则无法连接成功：[Remove element from array shell](http://stackoverflow.com/questions/16860877/remove-element-from-array-shell)
    delete=(${HostIP})
    host=( "${host[@]/$delete}" )
    echo "当前需要测试SSH的节点有：${host}"
    # 开始测试，测试正常将返回1，如果有一个节点无法ssh通就表示失败（注意：参数中包含空格！）
    ssh2nodes ${ssh_user} "${host}"
    ssh_result=`echo $?`
    # if判断的时候，注意格式，需要留出空格才行
    if [ ${ssh_result} == "1" ]
    then
        echo "SSH链接测试成功，开始执行后续组件启动"
    else
        echo "SSH链接测试失败，请检查是否有节点关闭或者无法连接，然后重新执行服务启动"
    fi

    ################# 执行步骤 #################
    # （1）根据当前角色判断是否需要启动mysql
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点服务启动"
        startMySQL
        echo "执行MySQL节点服务启动成功"
    else
        echo "不需要执行MySQL节点服务启动"
    fi

    # （2）从当前节点启动整个zookeeper集群服务
    zookeeper_node=${RoleListDict["zookeeper"]}
    echo "开始启动zookeeper集群各个节点"
    startup_Cluster_zookeeper "${zookeeper_node}"
    iszookeeperclusterup=`echo $?`
    # if判断的时候，注意格式，需要留出空格才行
    if [ ${iszookeeperclusterup} == "0" ]
    then
        echo "执行zookeeper集群启动成功"
    else
        echo "执行zookeeper集群启动失败，请检查日志，重新启动"
    fi

    # （2）首先必须根据当前角色判断是否需要启动zookeeper
    #is_zookeeper=$(substr ${HostRole} "zookeeper")
    #if [ ${is_zookeeper} == "1" ]
    #then
    #    echo "开始执行zookeeper节点服务启动"
    #    /usr/local/hadoop/zookeeper-3.4.6/bin/zkServer.sh start
    #    echo "执行zookeeper节点服务启动成功"
    #else
    #    echo "不需要执行zookeeper节点服务启动"
    #fi

    # （3）根据当前角色判断是否需要启动HDFS
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点服务启动"
        hdfs_master_startup
        echo "执行HDFS主节点服务启动成功"
    else
        echo "不需要执行HDFS主节点服务启动"
    fi

    # （4）根据当前角色判断是否需要启动HBase
    is_HBase_master=$(substr ${HostRole} "hbase_master")
    if [ ${is_HBase_master} == "1" ]
    then
        echo "开始执行HBase主节点服务启动"
        hbase_master_startup
        echo "执行HBase主节点服务启动成功"
    else
        echo "不需要执行HBase主节点服务启动"
    fi

    
    # （5）根据当前角色判断是否需要启动kafka
    is_Kafka=$(substr ${HostRole} "kafka")
    if [ ${is_Kafka} == "1" ]
    then
        echo "开始执行kafka节点服务启动"
        startupKafka ${kafka_root_path}
        echo "执行kafka节点服务启动成功"
    else
        echo "不需要执行kafka节点服务启动"
    fi

    # （6）根据当前角色判断是否需要启动es
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES节点服务启动"
        startES
        echo "执行ES节点服务启动成功"
    else
        echo "不需要执行ES节点服务启动"
    fi

    # （7）根据当前角色判断是否需要启动filebeat
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat节点服务启动"
        startfilebeat
        echo "执行filebeat节点服务启动成功"
    else
        echo "不需要执行filebeat节点服务启动"
    fi

    # （7）根据当前角色判断是否需要启动logstash
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash节点服务启动"
        startlogstash
        echo "执行logstash节点服务启动成功"
    else
        echo "不需要执行logstash节点服务启动"
    fi

    # （8）根据当前角色判断是否需要启动kibana
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana节点服务启动"
        startkibana
        echo "执行kibana节点服务启动成功"
    else
        echo "不需要执行kibana节点服务启动"
    fi

    ################# 完成自动化部署 #################
    echo "当前节点服务启动结束"
}

# 服务停止：
function stop(){
    ################# 初始化全局变量 #################
    initEnv

    ################# 脚本中使用的静态变量 #################
    # kafka根路径：
    kafka_root_path='/usr/local/hadoop/kafka_2.11-0.9.0.1'

    ################# 获取当前角色 #################
    HostName=$(getCurrentHostName)
    HostIP=$(getCurrentIP)
    HostRole=$(getCurrentHOSTRole ${HostName})
    echo "当前节点的IP为：${HostIP}"
    echo "当前节点的hostname为：${HostName}"
    echo "当前节点的角色为：${HostRole}"

    ################# 执行步骤 #################
    # （1）根据当前角色判断是否需要关闭kafka
    is_Kafka=$(substr ${HostRole} "kafka")
    if [ ${is_Kafka} == "1" ]
    then
        echo "开始执行kafka节点关闭"
        #stopKafka ${kafka_root_path}
        echo "执行kafka节点关闭成功"
    else
        echo "不需要执行kafka节点启动"
    fi

    # （2）根据当前角色判断是否需要关闭HBase
    is_HBase_master=$(substr ${HostRole} "hbase_master")
    if [ ${is_HBase_master} == "1" ]
    then
        echo "开始执行HBase主节点关闭"
        hbase_master_stop
        echo "执行HBase主节点关闭成功"
    else
        echo "不需要执行HBase主节点关闭"
    fi

    # （3）根据当前角色判断是否需要关闭HDFS
    is_HDFS_master=$(substr ${HostRole} "hdfs_master")
    if [ ${is_HDFS_master} == "1" ]
    then
        echo "开始执行HDFS主节点关闭"
        hdfs_master_stop
        echo "执行HDFS主节点关闭成功"
    else
        echo "不需要执行HDFS主节点关闭"
    fi

    # （4）从当前节点停止整个zookeeper集群服务
    zookeeper_node=${RoleListDict["zookeeper"]}
    echo "开始启动zookeeper集群各个节点"
    stop_Cluster_zookeeper "${zookeeper_node}"
    iszookeeperclusterdown=`echo $?`
    # if判断的时候，注意格式，需要留出空格才行
    if [ ${iszookeeperclusterdown} == "0" ]
    then
        echo "执行zookeeper集群停止成功"
    else
        echo "执行zookeeper集群停止失败，请检查日志，重新停止"
    fi

    # （4）根据当前角色判断是否需要关闭zookeeper
    #is_zookeeper=$(substr ${HostRole} "zookeeper")
    #if [ ${is_zookeeper} == "1" ]
    #then
    #    echo "开始执行zookeeper节点服务关闭"
    #    /usr/local/hadoop/zookeeper-3.4.6/bin/zkServer.sh stop
    #    echo "执行zookeeper节点服务关闭成功"
    #else
    #    echo "不需要执行zookeeper节点关闭"
    #fi

    # （5）根据当前角色判断是否需要关闭mysql
    is_MySQL=$(substr ${HostRole} "mysql")
    if [ ${is_MySQL} == "1" ]
    then
        echo "开始执行MySQL节点服务关闭"
        stopMySQL
        echo "执行MySQL节点服务关闭成功"
    else
        echo "不需要执行MySQL节点服务关闭"
    fi

    # （6）根据当前角色判断是否需要关闭elk
    is_es=$(substr ${HostRole} "es")
    if [ ${is_es} == "1" ]
    then
        echo "开始执行ES节点服务关闭"
        stopES
        echo "执行ES节点服务关闭成功"
    else
        echo "不需要执行ES节点服务关闭"
    fi

    # （7）根据当前角色判断是否需要关闭filebeat
    is_filebeat=$(substr ${HostRole} "filebeat")
    if [ ${is_filebeat} == "1" ]
    then
        echo "开始执行filebeat节点服务关闭"
        stopfilebeat
        echo "执行filebeat节点服务关闭成功"
    else
        echo "不需要执行filebeat节点服务关闭"
    fi

    # （8）根据当前角色判断是否需要关闭logstash
    is_logstash=$(substr ${HostRole} "logstash")
    if [ ${is_logstash} == "1" ]
    then
        echo "开始执行logstash节点服务关闭"
        stoplogstash
        echo "执行logstash节点服务关闭成功"
    else
        echo "不需要执行logstash节点服务关闭"
    fi

    # （9）根据当前角色判断是否需要关闭kibana
    is_kibana=$(substr ${HostRole} "kibana")
    if [ ${is_kibana} == "1" ]
    then
        echo "开始执行kibana节点服务关闭"
        stopkibana
        echo "执行kibana节点服务关闭成功"
    else
        echo "不需要执行kibana节点服务关闭"
    fi
    

    ################# 完成自动化部署 #################
    echo "当前节点服务关闭结束"

}

# 入口函数：
function entery(){
    echo "执行安装，请输入：install "
    echo "执行服务启动，请输入：startup "
    echo "执行服务停止，请输入：stop"
    read -p "输入要进行的选项：" option

    # 根据用户输入选择要执行的操作是安装还是启动
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


################ Stage 8: 部署 ################
#testInit "node1"
entery
