import os
import sys
import shutil
import subprocess
import threading

"""
提供文件夹之间的同步功能，从source到destination的单向传输，保证destination和source之间的完全一致
"""

def errExit(msg):
    print("-" * 50)
    print("ERROR:")
    print(msg)
    sys.exit(1)

# 文件同步主函数：有一个问题，同步的时候无法删除已经不存在的文件夹
def sync(source_dir, target_dir):
    print("synchronize '%s' >> '%s'..." % (source_dir, target_dir))
    print("=" * 50)
    # 统计要同步的文件的数量和总大小
    sync_file_count = 0
    sync_file_size = 0

    for root, dirs, files in os.walk(source_dir):
        relative_path = root.replace(source_dir, "")
        if len(relative_path) > 0 and relative_path[0] in ("/", "\\"):
            relative_path = relative_path[1:]
        dist_path = os.path.join(target_dir, relative_path)

        if os.path.isdir(dist_path) == False:
            os.makedirs(dist_path)

        last_copy_folder = ""
        for fn0 in files:
            fn = os.path.join(root, fn0)
            fn2 = os.path.join(dist_path, fn0)
            is_copy = False
            if os.path.isfile(fn2) == False:
                is_copy = True
            else:
                statinfo = os.stat(fn)
                statinfo2 = os.stat(fn2)
                is_copy = (
                    round(statinfo.st_mtime, 3) != round(statinfo2.st_mtime, 3) \
                    or statinfo.st_size != statinfo2.st_size
                )

            if is_copy:
                if dist_path != last_copy_folder:
                    print("[ %s ]" % dist_path)
                    last_copy_folder = dist_path
                print("copying '%s' ..." % fn0)
                shutil.copy2(fn, fn2)
                sync_file_count += 1
                print("copying current file success "+str(os.stat(fn).st_size)+" bytes")
                sync_file_size += os.stat(fn).st_size

    if sync_file_count > 0:
        print("-" * 50)
    print("%d files synchronized!" % sync_file_count)
    if sync_file_size > 0:
        print("%d bytes." % sync_file_size)
    print("done!")


# 添加调用winrar压缩加密文件
def winrarCypto(password, rarfilename, floder):
    # 构造命令，调用WinRAR的命令行工具来压缩文件
    rar_command = "C:\Program Files\WinRAR\Rar.exe a -p{0} {1} {2}".format(password, rarfilename, floder)
    # 使用subprocess.Popen来替换os.system，这样更方便
    out = subprocess.Popen(rar_command, stdout=subprocess.PIPE)
    # 等待线程执行完毕
    streamdata = out.communicate()[0]
    rc = out.returncode
    # 检查当前的线程是否执行完毕
    if rc is 0:
        print("success")
    else:
        print("fail")


# 定义一个外部调用的回调函数
def popenAndCall(onExit, popenArgs):
    """
    Runs the given args in a subprocess.Popen, and then calls the function
    onExit when the subprocess completes.
    onExit is a callable object, and popenArgs is a list/tuple of args that
    would give to subprocess.Popen.
    """
    def runInThread(onExit, popenArgs):
        #print(*popenArgs)
        lines = []
        proc = subprocess.Popen(popenArgs, bufsize=1, stdin=open(os.devnull), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        proc.stdout.close()
        proc.wait()
        onExit()
        return
    thread = threading.Thread(target=runInThread, args=(onExit, popenArgs))
    thread.start()
    # returns immediately after the thread starts
    return thread


# 定义一个可以定时跑的同步并且打包的函数
def SyncAndCypto(source_dir,target_dir,backup_dir,password):
    sync(source_dir, target_dir)
    currentFlodersubfix,currentFloderName = os.path.split(target_dir)
    print(currentFloderName)
    backup_full_path = backup_dir+"\\"+currentFloderName
    rar_command = "C:\Program Files\WinRAR\Rar.exe a -p{0} {1} {2}".format(password, backup_full_path, target_dir)
    # 定义回调函数
    def onExit():
        print("i am out")
    # 调用对应的回调函数和命令
    popenAndCall(onExit,rar_command)

# 当前执行的主函数
if __name__ == "__main__":

    """program1：测试命令行输入路径的文件夹同步功能
    # 参数检查
    if len(sys.argv) != 3:
        if "-h" in sys.argv or "--help" in sys.argv:
            print(__doc__)
            sys.exit(1)
        errExit(u"invalid arguments!")
    source_dir, target_dir = sys.argv[1:]
    if os.path.isdir(source_dir) == False:
        errExit(u"'%s' is not a folder!" % source_dir)
    elif os.path.isdir(target_dir) == False:
        errExit(u"'%s' is not a folder!" % target_dir)
    # 开始执行
    sync(source_dir, target_dir)
    """


    """program2：测试文件夹同步功能
    # execute main process
    source_dir = "D:\\workspace\\OpenSource\\opencv\\"
    target_dir = "D:\\work_temp\\FileSync\\2\\"
    sync(source_dir, target_dir)
    """

    """program3：使用
    # test winrar call
    winrarCypto("123","D:\\work_temp\\FileSync\\test","D:\\work_temp\\FileSync\\2")
    """

    """program4：使用回调测试cmd命令调用
    # 准备参数
    password = "123"
    rarfilename = "D:\\work_temp\\FileSync\\test"
    floder = "D:\\work_temp\\FileSync\\2"
    rar_command = "C:\Program Files\WinRAR\Rar.exe a -p{0} {1} {2}".format(password, rarfilename, floder)
    # 定义回调函数
    def onExit():
        print("i am out")
    # 调用对应的回调函数和命令
    popenAndCall(onExit,rar_command)
    """

    # 准备参数
    source_dir = "D:\\workspace\\OpenSource\\opencv\\"
    target_dir = "D:\\work_temp\\FileSync\\2"
    backup_dir = "D:\\work_temp\\FileSync\\"
    password = "123"
    SyncAndCypto(source_dir, target_dir,backup_dir,password)
