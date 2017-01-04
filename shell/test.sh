# say hello
function sayHello(){
your_name="$1"
echo ${your_name}
greeting="hello, ${your_name} !"
echo ${greeting}
}
#sayHello $1

# 获取本机的hostname和IP
function get_hostip(){
    local_host="`hostname --fqdn`"
    echo ${local_host}
    local_ip=`ip addr | grep 'state UP' -A2 | tail -n1 | awk '{print $2}' | cut -f1  -d'/'`
    echo ${local_ip}
}
#get_hostip

# 从函数中的局部变量中获取结果：
function getCurrentHOSTRole(){
    currentHostRole="NTP_MASTER"
    echo ${currentHostRole}
}

# 判断当前主机名对应的角色
function NTP_MASTER_DEPLOY(){
currentHostRole=$(getCurrentHOSTRole)
echo ${currentHostRole}
# 然后判断
if [ "${currentHostRole}" == "NTP_MASTER" ]
then
     echo "get success"
else
     echo "get fail"
fi
}

#
function calFileLines(){
    file=$1
    lines=`awk "BEGIN {i=0} {i++} END{print i}" $file`
    echo ${lines}
}
file="/home/wentao/shell/ntp.conf"
calFileLines ${file}

# NTP服务器配置
function ntp_master_config(){
    file=$1
    sed -i '8 c restrict default nomodify' ${file}
    sed -i '/# Please consider joining the pool (http:\/\/www.pool.ntp.org\/join.html)./{N;N;N;N;s/.*/server 127.127.1.0\nfudge  127.127.1.0 stratum 10/}' ${file}
    ntpd -c /etc/ntp.conf -p /tmp/ntpd.pid
    chkconfig ntpd on
}
#file="/home/wentao/shell/ntp.conf"
#ntp_master_config ${file}

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
}
#file=""
#ntp_master_ip=""
#ntp_slaver_config ${file} ${ntp_master_ip}

# SSH到子节点，执行命令退出
function ssh_node(){
    USERNAME=$1
    HOSTS=$2
    SCRIPT="pwd; ls"
    for HOSTNAME in ${HOSTS} ; do
        ssh -l ${USERNAME} ${HOSTNAME} "${SCRIPT}"
    done
}
username='root'
host='192.168.15.133'
ssh_node ${username} ${host}


# kafka设置
function setup_kafka(){
    # 获取对应的主机名
    #ZOOKEEPER_NODES=$1
    #content=""
    #for node in ${ZOOKEEPER_NODES} ; do
    #    content=${content}${node}":2181\,"
    #done
    #length=`expr ${#content} - 2`
    #content=${content:0:$length}
    #echo $content
    # 修改文件命令，设置breaker的id为当前机器的hostname
    KAFKA_CONFIG=$1
    #sed -i "s/^zookeeper.connect=/zookeeper.connect=${content}/g" ${KAFKA_CONFIG}
    #tsed "/zookeeper.connect=localhost:2181/{s/.*/zookeeper.connect=$content/}" ${KAFKA_CONFIG}
    content="node1:2181,node2:2181,node3:2181"
    echo $content
    sed -e "s/zookeeper.connect=localhost:2181/$content/g" $KAFKA_CONFIG

}
zookeeper_nodes='node1 node2 node3'
kafka_config='server.properties'
setup_kafka ${zookeeper_nodes} ${kafka_config}

















