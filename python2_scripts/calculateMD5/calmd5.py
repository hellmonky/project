# -*- coding:utf-8 -*-

######################################################

import hashlib
import binascii
import zlib

# 求给定字符串的md5编码
String = "35823905886361"+"319520320"
md5=hashlib.md5(String.encode('utf-8')).hexdigest()
# c4972f4fdda928bcf6f7e8e77ebaccc0
print(md5)


#验证文件md5和sha1
file='D:\\bin\\fab_v1.4\\Fab.exe'
# 这儿存在性能问题，读入大文件并且计算的效率很低，非常消耗资源，最好判断一下文件大小
verifyfile=open(file,'rb')
filecontent = verifyfile.read() # 读入所有的文件内容
md5=hashlib.md5(filecontent).hexdigest()
sha1=hashlib.sha1(filecontent).hexdigest()
print(zlib.crc32(filecontent))
verifyfile.close()
print(md5)
print(sha1)



######################################################

# 第一步：导入gui支持包
from Tkinter import *

# 第二步：从Frame派生一个Application类，这是所有Widget的父容器：
class Application(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

	# 在GUI中，每个Button、Label、输入框等，都是一个Widget。Frame则是可以容纳其他Widget的Widget，所有的Widget组合起来就是一棵树。
    def createWidgets(self):
        self.helloLabel = Label(self, text='Hello, world!')
        self.helloLabel.pack()
        self.quitButton = Button(self, text='Quit', command=self.quit)
        self.quitButton.pack()
		
# 第三步：实例化Application，并启动消息循环：
app = Application()
# 设置窗口标题:
app.master.title('Hello World')
# 主消息循环:
app.mainloop()