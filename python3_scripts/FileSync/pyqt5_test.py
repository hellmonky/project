"""
测试python3.4主版本对应的pyqt5的程序编写
"""

# 引入系统组件
import sys
import os
# 引入需要的Qt组件
from PyQt5 import QtWidgets

# 引入自定义文件中的函数
import floderSync

# 定义一个基本的Qt程序主体作为测试
def firstQtTest(StrToShow):
    # 以class开头自定义一个Qt类型，括号中表示继承的Qt类型方法
    class mywindow(QtWidgets.QWidget):
        def __init__(self):
            super(mywindow,self).__init__()
    """
    开始一个主程序的编写
    """
    # 接受系统参数的入口
    app = QtWidgets.QApplication(sys.argv)
    # 构造自己定义的Qt类
    windows = mywindow()
    # 在自定义的窗口中绑定label类
    label=QtWidgets.QLabel(windows)
    # 设置文字的属性，可以使用html标签来定制显示内容
    label.setText("<p style='color: red; margin-left: 20px'><b>"+StrToShow+"</b></p>")
    # 调用自己定义Qt主类的显示
    windows.show()
    # 外部包装监控当前的窗口退出
    sys.exit(app.exec_())


# 定义一个比较复杂的界面程序
def secondQtTest():
    # 自定义窗口组件
    class Window( QtWidgets.QWidget ):
        def __init__(self):
            # 设置基本的窗口组件
            super(Window, self).__init__()
            self.setWindowTitle("hello")
            self.resize(500, 500)
            # 设置当前的窗口布局
            gridlayout = QtWidgets.QGridLayout()
            # 添加按钮
            self.button1 = QtWidgets.QPushButton("File")
            self.button2 = QtWidgets.QPushButton("Font")
            self.button3 = QtWidgets.QPushButton("Color")
            self.button4 = QtWidgets.QPushButton("Floder")
            # 将按钮布局在当前窗口中
            gridlayout.addWidget(self.button1)
            gridlayout.addWidget(self.button2)
            gridlayout.addWidget(self.button3)
            gridlayout.addWidget(self.button4)
            # 设置分隔符
            spacer = QtWidgets.QSpacerItem(200, 80)
            gridlayout.addItem(spacer, 3, 1, 1, 3)
            self.setLayout(gridlayout)
            # pyqt5之后的使用了qt5新规定的信号和槽函数，根据文档给出的实例编写
            self.button1.clicked.connect(self.OnButton1)
            self.button2.clicked.connect(self.OnButton2)
            self.button3.clicked.connect(self.OnButton3)
            self.button4.clicked.connect(self.OnButton4)

        # 定义按钮事件的响应函数
        # 打开文件功能
        def OnButton1(self):
            # 注意：pyqt5中的getOpenFileName和官方Qt中的实现不一样，pyqt中相当于是pyqt4中的getOpenFileNameAndFilter函数
            fileName, filter = QtWidgets.QFileDialog.getOpenFileName(self, 'Open')
            print(fileName)
            if fileName is not None:
                self.setWindowTitle(fileName)
        # 打开字体
        def OnButton2(self):
            font, ok = QtWidgets.QFontDialog.getFont()
            if ok:
                self.setWindowTitle(font.key())
        # 打开调色板
        def OnButton3(self):
            color = QtWidgets.QColorDialog.getColor()
            if color.isValid():
                self.setWindowTitle(color.name())
        # 打开文件夹
        def OnButton4(self):
            folder = QtWidgets.QFileDialog.getExistingDirectory()
            if folder.title() is not None:
                self.setWindowTitle(folder.title())

    # 调用自定义的Qt组件显示
    app = QtWidgets.QApplication(sys.argv)
    win = Window()
    win.show()
    app.exec_()


"""
结合自定义文件中实现的功能，构造一个功能性函数，主要完成以下功能：
（1）可以选中源文件夹和目标文件夹，并且获取路径正确；
（2）可以选择是否加密压缩当前文件夹；
（3）可以输入压缩备份的密码
"""
def FolderSyncTest():
    # 自定义窗口组件
    class Window(QtWidgets.QWidget):
        def __init__(self):
            # 设置基本的窗口组件
            super(Window, self).__init__()
            self.setWindowTitle("folder_sync")
            self.resize(500, 500)
            # 设置当前的窗口布局为网格布局
            gridlayout = QtWidgets.QGridLayout()                # 整体布局
            # 添加按钮
            self.sourceFolderButton = QtWidgets.QPushButton("sourceFolder")             # 设置源目录
            self.destinationFolderButton = QtWidgets.QPushButton("destinationFolder")   # 设置目标目录
            self.cyptoButton = QtWidgets.QPushButton("cypto")                           # 加密按钮
            self.syncButton = QtWidgets.QPushButton("sync")                             # 同步按钮
            # 添加文本显示框
            self.sourceFullPath = QtWidgets.QLineEdit()         # 源路径编辑
            self.destinationFullPath = QtWidgets.QLineEdit()    # 目标路径编辑
            self.passwordEdit = QtWidgets.QLineEdit()           # 加密密码编辑
            # 布局测试
            gridlayout.addWidget(self.sourceFolderButton, 0, 0)
            gridlayout.addWidget(self.sourceFullPath, 0, 1)
            gridlayout.addWidget(self.destinationFolderButton, 1, 0)
            gridlayout.addWidget(self.destinationFullPath, 1, 1)
            gridlayout.addWidget(self.passwordEdit, 2, 0)
            gridlayout.addWidget(self.cyptoButton, 2, 1)
            gridlayout.addWidget(self.syncButton, 3, 0)
            # 设置分隔符
            spacer = QtWidgets.QSpacerItem(200, 80)
            gridlayout.addItem(spacer, 3, 1, 1, 3)
            self.setLayout(gridlayout)
            # pyqt5之后的使用了qt5新规定的信号和槽函数，根据文档给出的实例编写
            self.sourceFolderButton.clicked.connect(self.OnSourceFolderButton)
            self.destinationFolderButton.clicked.connect(self.OnDestinationFolderButton)
            self.cyptoButton.clicked.connect(self.OnCyptoButton)
            self.syncButton.clicked.connect(self.OnSyncButton)

        # 定义按钮事件的响应函数
        # 打开源文件夹路径
        def OnSourceFolderButton(self):
            folder = QtWidgets.QFileDialog.getExistingDirectory()
            if folder.title() is not None:
                self.sourceFullPath.setText(folder.title())

        # 打开目的文件夹路径
        def OnDestinationFolderButton(self):
            folder = QtWidgets.QFileDialog.getExistingDirectory()
            if folder.title() is not None:
                self.destinationFullPath.setText(folder.title())

        # 开始加密按钮
        def OnCyptoButton(self):
            print("开始加密")
            currentFlodersubfix, currentFloderName = os.path.split(self.sourceFullPath.text())
            backup_full_path = self.destinationFullPath.text()+"\\"+currentFloderName
            print(backup_full_path)
            rar_command = "C:\Program Files\WinRAR\Rar.exe a -p{0} {1} {2}".format(self.passwordEdit.text(), backup_full_path, self.destinationFullPath.text())
            # 定义回调函数
            def onExit():
                print("完成加密备份")
            # 调用对应的回调函数和命令
            floderSync.popenAndCall(onExit, rar_command)

        # 开始同步按钮
        def OnSyncButton(self):
            print("开始同步")
            print(self.sourceFullPath.text())
            print(self.destinationFullPath.text())
            floderSync.sync(self.sourceFullPath.text(), self.destinationFullPath.text())
            print("完成同步")

    # 调用自定义的Qt组件显示
    app = QtWidgets.QApplication(sys.argv)
    win = Window()
    win.show()
    app.exec_()



# 当前执行的主函数
if __name__ == "__main__":

    # 测试基本Qt程序
    #firstQtTest("hello_first_PyQt5_program")

    # 测试文件打开功能
    #secondQtTest()

    # 测试综合功能
    FolderSyncTest()