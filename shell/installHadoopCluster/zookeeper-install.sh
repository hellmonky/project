#! /bin/bash

if [[ $# -lt 1 ]];then 
    echo "USAGE:`basename $0` [-q zookeeper_quorum_hostname] [-h help]" 
    exit 1 
fi    

while getopts :q:h opt; do
    case $opt in
	q)
	    zookeeper_quorum=$OPTARG
	    ;;
	h)
	    echo "USAGE:"
	    echo "#zookeeper-install.sh [-q zookeeper_quorum_hostname] "
	    ;;
	\?)
            echo "Invalid arg: $OPTARG"
	    echo "see -h"
            ;;
    esac
done	

zookeeper_home='/usr/local/hadoop/zookeeper-3.4.6'
zookeeper_config_folder=${zookeeper_home}/conf

if [ -f "${zookeeper_config_folder}/zoo.cfg" ]; then
	rm -f ${zookeeper_config_folder}/zoo.cfg
fi

cp ${zookeeper_config_folder}/zoo_sample.cfg ${zookeeper_config_folder}/zoo.cfg
echo ${zookeeper_config_folder}
zoo_cfg=${zookeeper_config_folder}/zoo.cfg



#if [ ! -d "${zookeeper_home}/data" ]; then  
#    mkdir "${zookeeper_home}/data"
#fi 

if [ -d "${zookeeper_home}/data" ]; then  
    rm -rf ${zookeeper_home}/data
fi 

mkdir "${zookeeper_home}/data"

zookeeper_data=${zookeeper_home}/data
touch ${zookeeper_data}/myid
#myid=`ip addr show|grep "inet "|grep -v "127.0.0.1"|grep -v "192.168.122.1"|grep -v "172.17.0.1"|cut -d: -f2|cut -d \/ -f1|cut -d. -f4|awk '{print $1}'`
this_hostname=`hostname`
myid=`grep ${this_hostname} /etc/hosts|awk '{print $1}'|cut -d. -f4`
echo -n "">$zookeeper_data/myid
echo ${myid}>$zookeeper_data/myid


if [[ $(echo ${zookeeper_quorum}) =~ "," ]]; then  
i=1
while((1==1))  
	do  
        split=`echo ${zookeeper_quorum}|cut -d "," -f$i`  
        if [ "${split}" != "" ]  
        then  
            ((i++))  
			id=`grep ${split} /etc/hosts|awk '{print $1}'|cut -d. -f4`		
			echo "server.${id}=${split}:2888:3888">>${zoo_cfg}
        else  
            break  
        fi  
	done
else
	echo ${zookeeper_quorum}>>${zoo_cfg}
fi  

sed -i -e "s@dataDir=\/tmp\/zookeeper@dataDir=${zookeeper_data}@g" ${zoo_cfg}

echo "set ${zoo_cfg} successful!"  
