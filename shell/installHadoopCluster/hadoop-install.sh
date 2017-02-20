#! /bin/bash

if [[ $# -lt 1 ]];then 
    echo "USAGE:`basename $0` [-m master_hostname] [-n secondary_namenode] [-s slave_hostname1,slave_hostname2,...] [-r replication_num]" 
    exit 1 
fi 

while getopts :m:n:s:r:h opt; do
    case $opt in
	m)
	    hadoop_master_hostname=$OPTARG
	    ;;
	n) 
	    hadoop_secondary_namenode_hostname=$OPTARG
	    ;;
    s)
        hadoop_slaves=$OPTARG
        ;;
	r)
		dfs_replication=$OPTARG
		;;
	h)
	    echo "USAGE:"
	    echo "#hadoop-install.sh [-m master_hostname] [-n secondary_namenode] [-s slave_hostname1,slave_hostname2,...] [-r replication_num]"
	    ;;
	\?)
            echo "Invalid arg: $OPTARG"
	    echo "see -h"
            ;;
    esac
done	

JAVA_HOME='/usr/java/jdk1.8.0_92/'
hdfs_data_dir='/home/hdfs'
hadoop_home='/usr/local/hadoop/hadoop-2.6.0'
hadoop_config_folder=${hadoop_home}/etc/hadoop
hdfs_site_xml=${hadoop_config_folder}/hdfs-site.xml
core_site_xml=${hadoop_config_folder}/core-site.xml
yarn_site_xml=${hadoop_config_folder}/yarn-site.xml
slaves=${hadoop_config_folder}/slaves
hadoop_env_sh=${hadoop_config_folder}/hadoop-env.sh

sed -i -e '/<configuration>/,/<\/configuration>/d' ${core_site_xml}
echo -e "<configuration>\n</configuration>" >> ${core_site_xml}
sed -i -e '/<configuration>/,/<\/configuration>/d' ${hdfs_site_xml}
echo -e "<configuration>\n</configuration>" >> ${hdfs_site_xml}
sed -i -e '/<configuration>/,/<\/configuration>/d' ${yarn_site_xml}
echo -e "<configuration>\n</configuration>" >> ${yarn_site_xml}
echo -n "">${slaves}

sed -i -e "/<configuration>/a\\\t<property>\n\t\t<name>fs.defaultFS<\/name>\n\t\t<value>hdfs:\/\/${hadoop_master_hostname}:9000<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>hadoop.tmp.dir<\/name>\n\t\t<value>file:${hadoop_home}\/tmp<\/value>\n\t\t<description>A base for other temporary directories.<\/description>\n\t<\/property>" ${core_site_xml}
echo "set ${core_site_xml} successful!"

sed -i -e "/<configuration>/a\\\t<property>\n\t\t<name>dfs.namenode.secondary.http-address<\/name>\n\t\t<value>${hadoop_secondary_namenode_hostname}:50090<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>dfs.namenode.name.dir<\/name>\n\t\t<value>file:${hdfs_data_dir}/dfs/name<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>dfs.datanode.data.dir<\/name>\n\t\t<value>file:${hdfs_data_dir}/dfs/data<\/value>\n\t<\/property>\n\t<property>\n\t\t<name>dfs.replication<\/name>\n\t\t<value>${dfs_replication}<\/value>\n\t<\/property>" ${hdfs_site_xml}

echo "set ${hdfs_site_xml} successful!"

sed -i -e "/<configuration>/a\\\t<property>\n\t\t<name>yarn.resourcemanager.hostname<\/name>\n\t\t<value>${hadoop_master_hostname}<\/value>\n\t<\/property>" ${yarn_site_xml}
echo "set ${yarn_site_xml} successful!"

sed -i "s@\${JAVA_HOME}@${JAVA_HOME}@g" ${hadoop_env_sh}
echo "set ${hadoop_env_sh} successful!"


#sed -f test.sed $hive_site_conf > $hadoop_config_folder/text1.xml

if [[ $(echo ${hadoop_slaves}) =~ "," ]]; then  
	i=1
	while((1==1))  
		do  
			split=`echo ${hadoop_slaves}|cut -d "," -f$i`  
			if [ "${split}" != "" ]  
			then  
					((i++))  
			echo ${split}>>${slaves}  
			else  
					break  
			fi  
		done
else
	echo ${hadoop_slaves}>>${slaves}
fi  


echo "set ${slaves} successful!"  
