"""
使用watchdog来监控文件变化，然后获取变化列表；
然后使用path.py库来根据变化列表来动态的操作文件，例如增删查改等
"""

# 从watchdog的官方例程入手：
import sys
import os
import time
import re
import logging
# 调用watchdog模块
import watchdog
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
# 调用path.py模块
from path import Path


# 1 官方实例主要过程
def watchdog_test(input_dir):
    # 设置log文件的格式
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S')
    # 根据给定的路径进行监控，如果当前的路径合法就使用，否则默认监控当前文件夹
    if os.path.exists(input_dir):
        path = input_dir
    else:
        path = "."
    # 创建需要的日志事件
    event_handler = watchdog.events.LoggingEventHandler()
    # 创建监视器，然后添加任务
    observer = watchdog.observers.Observer()
    # 在监视器中添加日志事件和监控目录
    observer.schedule(event_handler, path, recursive=True)
    # 开始监控
    observer.start()
    # 开始每隔1秒定时监控
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    #
    observer.join()


# 2 正则表达式匹配测试
def regex_test():
    regex_str = ".*.txt$"
    dstStr = "CMakeLists.txt"
    compiled_regexStr = re.compile(regex_str)
    print(compiled_regexStr)
    match_result = re.match(compiled_regexStr, dstStr)
    print(match_result)


""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
# 一下为自己根据watchdog修改的主要类
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""


# 自定义一个类来调用watchdog来获取标准的操作过程：对应的文件夹和操作列表，或者就是一个连续的流，每次都处理一个文件变化
class standardFileMonitor(FileSystemEventHandler):
    # 自定义类的初始化函数
    def __init__(self):
        FileSystemEventHandler.__init__(self)

    # 内部变量，用于存储匹配正则表达式
    filter_map = [".*.txt$"]

    # 设置过滤条件
    def set_Filter(self, input_filter_regex):
        self.filter_map.append(input_filter_regex)

    # 获取外部设置的过滤条件
    def get_FilterRegex(self):
        return self.filter_map

    """
    功能：重载watchdog的on_created函数，来实现捕获新建文件事件
    输入参数：当前的监控句柄，当前的文件变化事件，当前的文件过滤条件
    """
    def on_created(self, event):
        # 获取当前文件变化的带有路径的完整文件名称
        file_name = event.src_path
        # print('--  '+file_name)
        # 设置一个获取成功文件
        operateFile = None
        # 获取当前设置的过滤条件
        filter_map = self.get_FilterRegex()
        # 检查当前的文件是否是过滤列表中的类型
        if file_name is not None:
            for key in filter_map:
                match_result = re.match(key, file_name)
                if match_result:
                    operateFile = file_name
                    break
                else:
                    print("当前文件不符合要求："+file_name)
        # 然后对符合要求的文件进行处理
        if operateFile is not None:
            try:
                print("检测到文件变化成功，操作文件为："+operateFile)
                # 调用文件同步功能来添加文件
            except Exception:
                print(Exception)

    """
    重载dispatch函数，接受所有的文件变化，主要用于日志记录
    """
    def on_deleted(self, event):
        # 获取当前文件变化的带有路径的完整文件名称
        file_name = event.src_path
        # 通知文件同步的删除操作
        print("删除文件："+file_name)

    """
    重载on_modified函数，接受所有的文件变化，主要用于日志记录
    """
    def on_modified(self, event):
        file_name = event.src_path
        # 通知文件同步的添加操作，并且最好有一个文件路径变化图
        print(file_name)

    """
    重载on_modified函数，接受所有的文件变化，主要用于日志记录
    """
    def on_moved(self, event):
        file_name = event.src_path
        # 文件的移动可以有两种方式实现，复制和删除，直接移动


# 自定义的调用类函数，用于外部包装使用
def MonitorFile():
    # 定义log的基本设置
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(message)s',datefmt='%Y-%m-%d %H:%M:%S')
    # 实例化一个自定义事件
    event_handler = standardFileMonitor()
    # 调用自定义事件中的辅助函数，添加过滤条件
    event_handler.set_Filter("md")
    # 实例化监控器，然后添加事件到监控器中，开始监控
    observer = Observer()
    observer.schedule(event_handler, watchDir, recursive=True)
    observer.start()
    print('Watching...')
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()


# 自己使用path.py封装的文件操作类
class standardFileOperator:

    # 初始化函数
    def __init__(self):
        print("init successful")

    # 内部变量
    operationPath = None        # 当前操作文件路径
    operationPathList = []      # 当前操作文件路径列表

    # 设置过滤条件
    def set_operationPath(self, path):
        self.operationPath = path

    # 获取外部设置的过滤条件
    def get_operationPath(self):
        return self.operationPath

    # 根据给定的完整路径删除文件
    def deleteFileWithFullPath(self, deletePath):
        # 获取当前的路径
        d = Path(deletePath)
        # 判断当前路径是否可以正常访问，如果可以就在当前路径下操作，否则报错或者创建路径
        path_state = d.access(os.F_OK)
        if path_state is True:
            print(d.abspath())
            # 获取当前路径下所有的文件个数
            num_files = len(d.files())
            print("当前共有文件："+str(num_files)+"个")
            # 然后遍历当前所有的文件
            for file in d.files():
                print(file)
            # 然后获取当前路径下的所有文件夹
            num_dirs = len(d.dirs())
            print("当前共有文件夹："+str(num_dirs)+"个")
            # 然后遍历当前的文件夹
            for current_dir in d.dirs():
                print(current_dir)



# 自定义文件操作测试函数
def OperationFile(path):
    operator = standardFileOperator()
    operator.set_operationPath(path)
    operator.deleteFile()



# 测试入口
if __name__ == "__main__":
    # 设置监控目录的路径
    watchDir = "D:\\work_temp\\FileSync\\1\\"

    # 1 官方调用测试
    #watchdog_test(watchDir)

    # 2 python3的re模块测试
    #regex_test()

    # 3 自定义文件夹监控测试
    #MonitorFile()

    # 4 自定义文件操作测试
    OperationFile(watchDir)

