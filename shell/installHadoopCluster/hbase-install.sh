#! /bin/bash

if [[ $# -lt 1 ]];then 
    echo "USAGE:`basename $0` [-m master_hostname] [-s regionserver_hostname1,regionserver_hostname2,...] [-z zookeeper_node_hostname1,zookeeper_node_hostname2,...]" 
    exit 1 
fi 

while getopts :m:z:s:h opt; do
    case $opt in
	m)
	    hbase_master_hostname=$OPTARG
	    ;;
	z) 
	    zookeeper_quorum=$OPTARG
	    ;;
    	s)
            hbase_regionservers_hostname=$OPTARG
            ;;
	h)
	    echo "USAGE:"
	    echo "#hadoop-install.sh -m [master_hostname] -n [secondary_namenode] -s [slave_hostname1,slave_hostname2,...]"
	    ;;
	\?)
            echo "Invalid arg: $OPTARG"
	    echo "see -h"
            ;;
    esac
done	

JAVA_HOME='/usr/java/jdk1.8.0_92/'
hbase_home='/usr/local/hadoop/hbase-1.2.0'
hbase_config_folder=${hbase_home}/conf
hbase_env_sh=${hbase_config_folder}/hbase-env.sh
hbase_site_xml=${hbase_config_folder}/hbase-site.xml
regionservers=${hbase_config_folder}/regionservers
zookeeper_data_dir=/usr/share/hadoop/zookeeper-3.4.6/data

sed -i -e '/<configuration>/,/<\/configuration>/d' ${hbase_site_xml}
echo -e "<configuration>\n</configuration>" >> ${hbase_site_xml}
echo -n "">${regionservers}

sed -i -e "/<configuration>/a\\\t<property>\n\t\t<name>hbase.rootdir<\/name>\n\t\t<value>hdfs://${hbase_master_hostname}:9000/hbase<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>hbase.cluster.distributed<\/name>\n\t\t<value>true<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>hbase.zookeeper.quorum<\/name>\n\t\t<value>${zookeeper_quorum}<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>hbase.zookeeper.property.dataDir<\/name>\n\t\t<value>${zookeeper_data_dir}<\/value>\n\t</property>" ${hbase_site_xml}
echo "set ${hbase_site_xml} successful!"

sed -i -e "s@# export JAVA_HOME=\/usr\/java\/jdk1.6.0\/@export JAVA_HOME=${JAVA_HOME}@g" ${hbase_env_sh}
sed -i -e "s@# export HBASE_MANAGES_ZK=true@export HBASE_MANAGES_ZK=false@g" ${hbase_env_sh}
echo "set ${hbase_env_sh} successful!"


i=1
while((1==1))  
do  
	regions=$(echo ${hbase_regionservers_hostname} | grep ",")
	if [[ "${regions}" == "" ]]
	    then
            echo ${hbase_regionservers_hostname}>>${regionservers} 
	    break
	fi

        split=`echo ${hbase_regionservers_hostname}|cut -d "," -f$i`  
        if [ "${split}" != "" ]  
        then  
                ((i++))  
		echo ${split}>>${regionservers}  
        else  
                break  
        fi  
done
echo "set ${regionservers} successful!"  
