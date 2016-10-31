# 使用base64来处理个人文件的加密和解密

import sys
import os
import base64

# 定义一个函数，输入文件路径，输出读入的二进制结果
def readFileToBytes(filePath):
    f = open(filePath, 'rb')  # 读取二进制文件，然后转换编码
    contents = f.read()
    f.close()
    return contents

# 使用base64加密bytes类型数据，然后返回加密结果
def encrypToBytes(content):
    result = base64.encodebytes(content)
    return result

# 解密bytes的数据，然后返回解密结果
def decrypToBytes(content):
    result = base64.decodebytes(content)
    return result

# 将二进制内容按照UTF-8编码写入到文件中：
def writeBinaryToFile(byteContents, file):
    writeFileData = open(file, 'wb')
    writeFileData.write(byteContents)
    writeFileData.close()


# 主要执行过程
def main():
    # 检查传递参数个数是否正确
    if len(sys.argv) != 4:
        #print("[+] Usage:", sys.argv[1], "(/path/to/input_file)", sys.argv[2], "(option)", sys.argv[3], "(/path/to/output_file)\n")
        sys.exit(0)

    '''
    # 检查参数
    print(sys.argv[0])
    print(sys.argv[1])
    print(sys.argv[2])
    print(sys.argv[3])
    '''

    # check gesture.key file
    if not os.path.isfile(sys.argv[1]):
        print("[e] Cannot access to file", sys.argv[1])
        sys.exit(-1)
    # 读入文件
    byteContents = readFileToBytes(sys.argv[1])
    # 然后判断当前的调用是用来加密还是解密
    condition = sys.argv[2]
    # 注意字符串的比较
    if str(condition) == "encode":
        encodeResult = encrypToBytes(byteContents)
        # 然后写入到文件中
        try:
            writeBinaryToFile(encodeResult, sys.argv[3])
        except Exception :
            print("[e] output file:", sys.argv[3], "is wrong\n")
        print("success")
    elif str(condition) == "decode":
        decodeResult = decrypToBytes(byteContents)
        # 然后写入到文件中
        try:
            writeBinaryToFile(decodeResult, sys.argv[3])
        except Exception:
            print("[e] output file:", sys.argv[3], "is wrong\n")
        print("success")
    else:
        print("[e] option:", sys.argv[2], "is wrong\n")
        print("fail")


# 主函数入口
if __name__ == "__main__":
    main()
