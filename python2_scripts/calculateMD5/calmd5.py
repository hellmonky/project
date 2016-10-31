# -*- coding:utf-8 -*-

######################################################

import hashlib
import binascii
import zlib

# ������ַ�����md5����
String = "35823905886361"+"319520320"
md5=hashlib.md5(String.encode('utf-8')).hexdigest()
# c4972f4fdda928bcf6f7e8e77ebaccc0
print(md5)


#��֤�ļ�md5��sha1
file='D:\\bin\\fab_v1.4\\Fab.exe'
# ��������������⣬������ļ����Ҽ����Ч�ʺܵͣ��ǳ�������Դ������ж�һ���ļ���С
verifyfile=open(file,'rb')
filecontent = verifyfile.read() # �������е��ļ�����
md5=hashlib.md5(filecontent).hexdigest()
sha1=hashlib.sha1(filecontent).hexdigest()
print(zlib.crc32(filecontent))
verifyfile.close()
print(md5)
print(sha1)



######################################################

# ��һ��������gui֧�ְ�
from Tkinter import *

# �ڶ�������Frame����һ��Application�࣬��������Widget�ĸ�������
class Application(Frame):
    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

	# ��GUI�У�ÿ��Button��Label�������ȣ�����һ��Widget��Frame���ǿ�����������Widget��Widget�����е�Widget�����������һ������
    def createWidgets(self):
        self.helloLabel = Label(self, text='Hello, world!')
        self.helloLabel.pack()
        self.quitButton = Button(self, text='Quit', command=self.quit)
        self.quitButton.pack()
		
# ��������ʵ����Application����������Ϣѭ����
app = Application()
# ���ô��ڱ���:
app.master.title('Hello World')
# ����Ϣѭ��:
app.mainloop()