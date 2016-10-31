from ctypes import cdll

# 读入指定路径下的动态链接库
lib = cdll.LoadLibrary("D:/workspace/project/rust_scripts/lib_test/target/release/lib_test_out.dll")

# 调用了动态库中的指定名称为process的函数
lib.process()

# 执行完毕后返回，打印
print("done!")