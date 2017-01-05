#!/bin/bash
   


# 安装ELK
function installELK(){
    #elasticsearch安装位置
    es_tarpath='/usr/local/elasticsearch-1.6.0.tar.gz'
    desfilepath='/usr/local/'
    filename_es='elasticsearch-1.6.0'

    # 解压tar包到指定文件夹
    tar -zvxf ${es_tarpath} -C ${desfilepath}  
	
	#es的读写权限
	chmod -R 775 ./elasticsearch-1.6.0	
	
	#logstash安装位置
    log_tarpath='/usr/local/logstash-1.5.6.tar.gz'
    desfilepath='/usr/local/'
    filename_log='logstash-1.5.6'

    # 解压tar包到指定文件夹
    tar -zvxf ${log_tarpath} -C ${desfilepath}
	
	#kibana安装位置
    kb_tarpath='/usr/local/kibana-4.0.3-linux-x64.tar.gz'
    desfilepath='/usr/local/'
    filename_kb='kibana-4.0.3-linux-x64'

    # 解压tar包到指定文件夹
    tar -zvxf ${kb_tarpath} -C ${desfilepath}  

	#filebeat安装位置
    fb_tarpath='/usr/local/filebeat-1.0.0-x86_64.tar.gz'
    desfilepath='/usr/local/'
    filename_fb='filebeat-1.0.0-x86_64'

    # 解压tar包到指定文件夹
    tar -zvxf ${fb_tarpath} -C ${desfilepath}  
}

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

function startES(){
    /usr/local/elasticsearch-1.6.0/bin/elasticsearch  -d
}
function startlogstash(){
   cd /usr/local/logstash-1.5.6/
   ./bin/logstash -f ./beatsfinal.conf &
}
function startkibana(){
    cd /usr/local//kibana-4.0.3-linux-x64/
    ./bin/kibana &
}
function startfilebeat(){
  cd /usr/local/filebeat-1.0.0-x86_64/
  ./filebeat -e -c ./filebeat.yml   &
}


function stopES(){
    ps -ef | grep elasticsearch | awk '{print $2}'|xargs kill 
	
}
function stoplogstash(){
    ps -ef | grep logstash | awk '{print $2}'|xargs kill
}
function stopkibana(){
    ps -ef | grep kibana | awk '{print $2}'|xargs kill
}
function stopfilebeat(){
   ps -ef | grep filebeat | awk '{print $2}'|xargs kill
}



#installELK

#updateYML
#startES
#startlogstash
startkibana
#startfilebeat
stopES
stoplogstash
stopkibana
stopfilebeat