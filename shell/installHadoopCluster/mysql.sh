#!/bin/bash
# 安装MySQL
function installMySQL(){
    #安装位置
    tarfilepath='/usr/local/mysql-5.7.6-m16-linux-glibc2.5-x86_64.tar.gz'
    desfilepath='/usr/local/'
    filename='mysql-5.7.6-m16-linux-glibc2.5-x86_64'

    # 解压tar包到指定文件夹
    tar -zvxf ${tarfilepath} -C ${desfilepath}  
    cd ${desfilepath}
    mv ${filename}  ./mysql
    
    # 添加服务初始化表
    groupadd mysql
    useradd -r -g mysql mysql
    chown -R mysql:mysql ./mysql
    ./mysql/bin/mysqld --initialize-insecure --user=mysql  
	
    #需要更新my.cnf文件
    cnfpath='/usr/local/my.cnf'
    cp  ${cnfpath} /etc/my.cnf
}

#installMySQL

function startMySQL(){
     cd /usr/local/mysql/support-files/
	 ./mysql.server start
}

function stopMySQL(){
     cd /usr/local/mysql/support-files/
	 ./mysql.server stop
}

#startMySQL
stopMySQL