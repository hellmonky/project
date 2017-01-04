#! /bin/bash

sh hadoop-install.sh -m master -n smaster -s slave01,slave02,slave03
sh hbase-install.sh -m hmaster -s hslave01,hslave02 -z zk1,zk2,zk3
sh zookeeper-install.sh -q zk1,zk2,zk3
