# -*- coding: utf-8 -*-

# 主要实现功能：从指定的网页获取要匹配的内容，然后调用wget下载对应的内容

# 基本网络通信库
import urllib2
# 基本的正则表达式处理库
import re
# 外部可执行文件调用
from subprocess import call
# 系统支持库
import sys


# 对SourceForge上的资源进行下载了，但是现在还需要设置一些参数
# （1）下载页面地址 （2）下载的文件夹名称
def sourceforgeD(url, foldername):
	MainUrl = url
	req = urllib2.Request(MainUrl);
	resp = urllib2.urlopen(req);
	respHtml = resp.read();
	pattern_findLink = '<a href="(.+)"\s+?.\s+?title="Click to download .+\s+.*>\s+.*</a>'
	findAllContent  = re.findall(pattern_findLink, respHtml);
	download_path = "D:\\wget\\download\\"+foldername;
	if(findAllContent):
		for eachContent in findAllContent:
			# 构造外部调用命令
			commandStr = "D:\\wget\\wget64.exe -c -P " + download_path + " "+eachContent;
			print "the following will be download : "+ eachContent;
			call(commandStr, shell=True);
			
# 外部调用的主函数
if __name__=="__main__":
	# 读入参数
	for i in range(1, len(sys.argv)):
		# 参数个数检查
		if(len(sys.argv)==3):
			# 执行操作
			print sys.argv[0],sys.argv[1],sys.argv[2];
			sourceforgeD(sys.argv[1],sys.argv[2]);