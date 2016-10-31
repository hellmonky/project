import urllib.request
from bs4 import BeautifulSoup
import os
import time
import random
import re
import unicodedata
import string
import http.client


# 首先是一些辅助函数的定义：


# 获取？
def extract(text, sub1, sub2):
    return text.split(sub1, 1)[-1].split(sub2, 1)[0]


# 去除标题中的非法字符 (Windows)
def validateTitle(title):
    rster = r"[\/\\\:\*\?\"\<\>\|\.]"  # '/\:*?"<>|.'
    new_title = re.sub(rster, "", title)
    return new_title

# 将字符串转换为合法的路径
validFilenameChars = "-_.() %s%s" % (string.ascii_letters, string.digits)
def removeDisallowedFilenameChars(filename):
    cleanedFilename = unicodedata.normalize('NFKD', filename).encode('ASCII', 'ignore')
    return ''.join(c for c in cleanedFilename if c in validFilenameChars)


# 正则表达式转换URL中的中文字符为百分号编码格式，防止链接无法打开
def URLchineseTranslate(url):
    # 定义了一个正则处理的回调函数，用于处理正则匹配的结果
    def chage(m):
        return urllib.parse.quote(m.group(1))
    # 正则匹配后获取匹配结果然后进行编码处理
    # 其中中文的UTF8编码范围参考：
    transferURL = re.sub(r"([\u4e00-\u9fa5]+[\u3002\uFF1F\uFF01\uFF0C\u3001\uFF1B\uFF1A\u300C\u300D\u300E\u300F\u2018\u2019\u201C\u201D]*)", chage, url)
    return transferURL


# 获取原给定网址的原始内容
def getOriginalContent(url):
    # 初始化返回内容
    content = None
    # 加入当前网址的有效性检查，来自django url validation regex
    regex_legal = re.compile(
            r'^(?:http|ftp)s?://' # http:// or https://
            r'(?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+(?:[A-Z]{2,6}\.?|[A-Z0-9-]{2,}\.?)|' #domain...
            r'localhost|' #localhost...
            r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})' # ...or ip
            r'(?::\d+)?' # optional port
            r'(?:/?|[/?]\S+)$', re.IGNORECASE)
    # 判断当前的链接是否是一个合法的链接
    if regex_legal.match(url) is not None:
        proxy = urllib.request.ProxyHandler({'http': '127.0.0.1:5040', 'https': '127.0.0.1:5040'})
        opener = urllib.request.build_opener(proxy)
        urllib.request.install_opener(opener)
        # 伪造浏览器头，最关键的还是User-Agent内容的伪造，还有就是请求的速度一定要是随机的，不然会被干掉IP
        # Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36
        # Mozilla/5.0 (Windows NT 6.1; WOW64; rv:23.0) Gecko/20100101 Firefox/23.0
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36'}
        # 然后处理当前链接中的中文字符
        finalURL = URLchineseTranslate(url)
        req = urllib.request.Request(url=finalURL, headers=headers)
        # 进行异常链接判断
        try:
            # 注意对于中文链接无法正确打开，例如：http://chuantupian.com/8/uploads/2016/01/爱只一个字，不需多情痴，两盒杜蕾斯，黑尽天下比.jpg
            response = urllib.request.urlopen(req)
        except urllib.error.URLError as e:
            print('The server couldn\'t fulfill the request.')
            print('Error code: ', e.reason)
            content = None
        except urllib.error.HTTPError as e:
            print(e.reason)
            content = None
        else:
            print('get content success')
            # 注意读取的时候也需要检查，否则会遇到读取不完整失败
            try:
                content = response.read()
            except http.client.IncompleteRead as readError:
                print(readError.partial)
                content = None
    return content



# 获取解码后的网页内容
def getContentWithURL(url):
    # 初始化返回值
    decode_content = None
    # 获取网页内容
    content = getOriginalContent(url)
    # 判断当前的返回是否正常
    if content is not None:
        # 获取网页的编码
        encoding = extract(str(content).lower(), 'charset=', '"')
        # 然后将网页获取内容根据当前的编码进行解码
        if encoding.__eq__('gb2312'):
            # note that Python3 does not read the html code as string
            # but as html code bytearray, convert to string with
            decode_content = content.decode('GBK')
            #decode_content = content.decode(encoding)
        else:
            print("Encoding type not found!")
    return decode_content



def crawerEach(url,urldir):
    # 首先获取输入网址的内容
    content = getContentWithURL(url)
    # 继续解析网页内容，使用GBK编码来保证中文不会乱码，然后获取对应的标签参数值
    soup = BeautifulSoup(content, from_encoding='GBK')
    # 获取指定的标签
    items = soup\
        .find('body')\
        .find('div', id='main')\
        .find(name='div', attrs={"class":"t", "style":"margin:3px auto"})\
        .find('table', id='ajaxtable')\
        .find("tbody", attrs={"style":"table-layout:fixed;"})\
        .findAll(name="tr", attrs={"class":"tr3 t_one"})
    for item in items:
        target = item.find(name='td', attrs={"style":"text-align:left;padding-left:8px"})
        if target is not None:
            target = target.find('h3').find('a')
            if target.u is None and target.b is None and target.font is None:
                urldir[target.text] = "http://www.t66y.com/"+target.get('href')
    return urldir



#爬下所有文章的标题的URL地址
def crawer(baseurl):
    # 获取当前网址的
    urldir = {}
    for i in range(5):
        url = baseurl+"&page="+str(i+1)
        print("=====================正在爬取第"+str(i+1)+"页=========")
        urldir = crawerEach(url, urldir)
    # 打开文件准备写入
    f = open("all.xml", 'w', encoding="utf-8")
    for key, url in urldir.items():
        firstColumn = "<article title="+"\""+key+"\">"
        secondColumn = "   "+"<url>"+url+"</url>"
        thirdColumn = "</article>"
        f.write(firstColumn+'\n'+secondColumn+'\n'+thirdColumn+'\n')
    f.close()


def getPicTest(baseurl, fileLocation):
    # 获取当前网址的
    urldir = {}
    for i in range(2):
        totalurl = baseurl+"&page="+str(i+1)
        print("=====================正在爬取第"+str(i+1)+"页=========")
        urldir = crawerEach(totalurl, urldir)
        for key, url in urldir.items():
            # 防止被干掉，这儿使用随机数来生成间隔时间
            sleepTime = random.randint(1, 10)
            time.sleep(sleepTime)
            print("正在处理页面名称："+key)
            # 注意：需要将网址转义，否则对于包含空格的会无法打开
            print(urllib.parse.quote(url))
            full_url = url.replace(" ", "%20")
            # 然后显示当前正在处理的网页地址
            print("正在处理页面地址："+full_url)
            content = getOriginalContent(full_url)
            if content is not None:
                soup = BeautifulSoup(content, from_encoding='GBK')
                images = soup.find_all('img', attrs={"style":"cursor:pointer"})
                if images is None:
                    print("当前页面没有发现图像文件")
                # 对当前页面创建子文件夹
                # 首先将当前的普通字符串转换为路径合法的名称
                key = validateTitle(key)
                print('creat floder name is : '+key)
                #key = base64.urlsafe_b64encode(key)
                currentPath = fileLocation + key
                # 判断当前是否可以正确的建立文件夹，这儿需要注意，如果已经存在文件夹，就需要根据当前已有的下载文件继续编号下载，而不是覆盖
                if not os.path.exists(currentPath):
                    try:
                        os.mkdir(currentPath)
                    except OSError as exc:
                        raise exc
                    # 然后再下载文件到对应的文件夹中
                    print("开始下载图片。。。。。")
                    i = 0
                    for image in images:
                        currentLink = image.get('src')
                        print(currentLink)
                        # 然后判断是那种图片类型，然后根据后缀来确认保存的对应的图片类型
                        if(currentLink.find('.jpg')>0):
                            conn = getOriginalContent(currentLink)
                            # 检查当前的获取文件是否为空，如果为空就不写入
                            if conn is not None:
                                f = open(currentPath+"\\"+str(i)+".jpg", 'wb')
                                f.write(conn)
                                f.close()
                                i=i+1
                        if(currentLink.find('.gif')>0):
                            conn = getOriginalContent(currentLink)
                            if conn is not None:
                                f = open(currentPath+"\\"+str(i)+".gif", 'wb')
                                f.write(conn)
                                f.close()
                                i=i+1
                else:
                    print('已经存在文件夹，请检查后重新下载')





#获得文章内容
def getContent(soup , author ,url, pageAccount):
    contents = soup.body.find(name='div',attrs={'id':'main'}).findAll(name='div',attrs={'class':'t t2'})
    tid = url[-12:]
    print (tid)
    #获得首页的文章内容
    for item in contents:
        if(item.find('table').find(name='tr',attrs={'class':'tr3 tr1'}).find('font').b.text == author):
            content = item.table.find(name='tr',attrs={'class':'tr3 tr1'}).find(name='th',attrs={'class':'r_one'}
                                                                            ).table.tr.td.find(name='div',attrs={'class':'tpc_content'}).text
            writeContent(content)
            print(content)
            print("")
    pageInt = int(pageAccount)
    i = 2
    while i<=pageInt:
        pageUrl = "http://wo.yao.cl/read.php?tid=" + tid + "&page=" + str(i)
        print(pageUrl)
        getAuthorFloorContent(pageUrl,author)
        i=i+1
    print(pageUrl)

#把内容写入文件
def writeContent(content):
    f=open('content1.txt','a',encoding='utf-8')
    f.write(content)
    f.write('\n')
    f.write('\n')
    f.write('\n')
    f.write('\n')
    f.close()


#获得第2页以后的页面的作者的楼层中的内容
def getAuthorFloorContent(pageUrl,author):
    html = getContentWithURL(pageUrl)
    soup = BeautifulSoup(html)
    # 获得所有楼层
    contents = soup.body.find(name='div',attrs={'id':'main'}).findAll(name='div',attrs={'class':'t t2'})
    # 然后遍历当前的每一层获取内容
    for item in contents:
        #在所有楼层中选出作者的楼层
        if(item.find('table').find(name='tr',attrs={'class':'tr1'}).find(name='th',attrs={'class':'r_two'}).b.text == author):
            content = item.table\
                .find(name='tr',attrs={'class':'tr1'})\
                .find(name='th',attrs={'class':'r_one'})\
                .find(name='div',attrs={'class':'tpc_content'})\
                .text
            writeContent(content)
            print(content)
            print("")


#获得帖子中共有多少页
def getContentPage(soup):
    divItems = soup.body.find('div',id='main').findAll(name='div',attrs={'class':'t3'})
    #获得页数的节点
    pageAccounts = divItems[2].table.tr.td.find(name='div',attrs={'class':'pages'}).findAll(name='a',attrs={'style':None})
    pageAccount = pageAccounts[len(pageAccounts)-1].text
    print("页数为：" + pageAccount)
    return pageAccount



#获得作者名字
def getAuthor(soup):
    author = soup.body.find('div',id='main')\
        .find(name='div',attrs={'class':'t t2'})\
        .find('table')\
        .find(name='tr',attrs={'class':'tr3 tr1'})\
        .find('font')\
        .b.text
    print("作者为：" + author)
    return author


#获得文章
def getArtilcle(url):
    html = getContentWithURL(url)
    soup = BeautifulSoup(html)
    #取得帖子的页数
    account = getContentPage(soup)
    #取得文章的作者
    author = getAuthor(soup)
    #取得内容，并将内容存入txt
    content = getContent(soup , author ,url ,account)


#获得图片
def getPicture(url):
    resp = getContentWithURL(url)
    soup = BeautifulSoup(resp)
    contents = soup.body.find(name='div',attrs={'id':'main'}).findAll(name='div',attrs={'class':'t t2'})
    #获得网页内容
    for item in contents:
        pictures = item.table\
            .find(name='tr',attrs={'class':'tr3 tr1'})\
            .find(name='th',attrs={'class':'r_one'}).\
            table.tr.td\
            .find(name='div',attrs={'class':'tpc_content'})\
            .findAll(name='input')
        i = 0
        for tag in pictures:
            print(tag['src'])
            conn = urllib.request.urlopen(tag['src'])
            f=open(str(i)+".jpg",'wb')
            i=i+1
            f.write(conn.read())
            f.close()
    resp.close()



# 然后是测试程序调用入口
if __name__ == "__main__":


    #################################################################
    # 伪造浏览器头部测试
    url = 'http://chuantupian.com/8/uploads/2016/01/爱只一个字，不需多情痴，两盒杜蕾斯，黑尽天下比.jpg'
    #transferURL = 'http://chuantupian.com/8/uploads/2016/01/%E7%88%B1%E5%8F%AA%E4%B8%80%E4%B8%AA%E5%AD%97%EF%BC%8C%E4%B8%8D%E9%9C%80%E5%A4%9A%E6%83%85%E7%97%B4%EF%BC%8C%E4%B8%A4%E7%9B%92%E6%9D%9C%E8%95%BE%E6%96%AF%EF%BC%8C%E9%BB%91%E5%B0%BD%E5%A4%A9%E4%B8%8B%E6%AF%94.jpg'
    # 转换URL中的中文编码，否则一定会报错，而且一定要转换回来，不然就无法打开了
    # 定义了一个回调函数，用于处理正则匹配的结果
    def chage(m):
        return urllib.parse.quote(m.group(1))
    transferURL = re.sub(r"([\u4e00-\u9fa5]+[\u3002\uFF1F\uFF01\uFF0C\u3001\uFF1B\uFF1A\u300C\u300D\u300E\u300F\u2018\u2019\u201C\u201D]*)", chage, url)
    print(transferURL)
    # 然后再正常打开处理
    headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36'}
    req = urllib.request.Request(url=transferURL, headers=headers)
    response = urllib.request.urlopen(req)
    content = response.read()
    file = open('D:\\work_temp\\getweb\\1.jpg', 'wb')
    file.write(content)
    file.close()

    #crawer(base_test)


    #################################################################
    # 实测获取图片
    baseURL = 'http://www.t66y.com/'
    jstl = 'thread0806.php?fid=7' # 技术讨论区基础地址
    dstFileLocation = 'D:\\work_temp\\getweb\\测试下载图片2\\'
    base_test = baseURL+jstl
    getPicTest(base_test, dstFileLocation)