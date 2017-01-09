#!/bin/bash

function changeHostIP(){
    eth=$1
    IP=$2

    echo "${eth} 内容如下："
    cat ${eth}

    sed -i "7a IPADDR=${IP}" ${eth}
    systemctl restart  network.service
    echo "设置IP成功"
}


read -p "输入当前节点需要设置的IP地址：" IP
echo "确认当前输入IP : ${IP}"
ifcfg_file='/etc/sysconfig/network-scripts/ifcfg-ens18'
changeHostIP ${ifcfg_file} ${IP} 