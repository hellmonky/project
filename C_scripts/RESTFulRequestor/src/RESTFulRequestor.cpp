// C++封装的对RESTFul API调用类实现文件
#include "RESTFulRequestor.h"

// JSON转换接口
#include "AutoParser.cpp"

template<class T>
RESTFulRequestor<T>::RESTFulRequestor() {
	curl_global_init(CURL_GLOBAL_ALL);
	// 当前要访问的URL地址
	this->URLAddress = "";
	// 默认cookie文件路径为空
	this->cookiePath = "";	
	// 设置base64编码常量
	base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz"
								"0123456789+/";
	// 欢迎致辞
	std::cout << "welcome to use RESTFul API caller using C++ wrapper" << std::endl;
}

template<class T>
RESTFulRequestor<T>::~RESTFulRequestor() {
	// 清除curl操作.
	curl_global_cleanup();
	// 离开致辞
	std::cout << "finish wrapper caller" << std::endl;
}


/**********************************************************************************/
/*********************************base64编码*************************************/
/**********************************************************************************/
// base64加密
template<class T>
string RESTFulRequestor<T>::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++) {
				ret += base64_chars[char_array_4[i]];
			}

			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++) {
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++) {
			ret += base64_chars[char_array_4[j]];
		}

		while ((i++ < 3)) {
			ret += '=';
		}
	}
	return ret;
}

// base64解码
template<class T>
string RESTFulRequestor<T>::base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++) {
				char_array_4[i] = base64_chars.find(char_array_4[i]);
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++) {
				ret += char_array_3[i];
			}

			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++) {
			char_array_4[j] = 0;
		}

		for (j = 0; j < 4; j++) {
			char_array_4[j] = base64_chars.find(char_array_4[j]);
		}

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}
	return ret;
}


/**********************************************************************************/
/*********************************基本回调****************************************/
/**********************************************************************************/
// 回调函数：将返回数据写入到字符串缓存中
template<class T>
size_t RESTFulRequestor<T>::WriteCallback2Stream(void* buffer, size_t size, size_t nmemb, void *stream) {
	((std::string*)stream)->append((char*)buffer, size * nmemb);
	return size * nmemb;
}

// 回调函数：将返回数据写入到文件缓存中
template<class T>
size_t RESTFulRequestor<T>::WriteCallback2File(void* buffer, size_t size, size_t nmemb, void *stream) {
	FILE *fptr = (FILE*)stream;

	//fwrite(buffer, size, nmemb, fptr);
	//return size*nmemb;

	size_t written = fwrite(buffer, size, nmemb, fptr);
	return written;
}

// 回调函数：将获取的数据按照二进制流写入到文件中，这个函数执行时间比较长，最好开一下后台线程进行处理
template<class T>
size_t RESTFulRequestor<T>::WriteCallbackAsFile(void* buffer, size_t size, size_t nmemb, void *stream) {
	struct DownloadFileStruct *out = (struct DownloadFileStruct *)stream;
	if (out && !out->stream) {
		// 按照二进制格式打开文件进行写入
		out->stream = fopen(out->filename, "wb");
		if (!out->stream) {
			return -1;
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}


/**********************************************************************************/
/*********************************GET 请求****************************************/
/**********************************************************************************/
// 访问指定的URL，并且将返回内容回显到屏幕上
template<class T>
void RESTFulRequestor<T>::getGETResponse2Screen(string URLAddress) {
	// 初始化：
	this->curl = curl_easy_init();
	
	//
	if (this->curl != NULL) {
		// init to NULL is important 
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");

		//设置访问URL：CURLOPT_URL
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());

		//设置当前的用户端信息：用于伪造浏览器访问，有的网站检查这个项目来确定是否提供访问
		struct curl_slist *useragent = NULL;
		useragent = curl_slist_append(useragent, "Mozilla/5.0 (Windows; U; Windows NT 5.1; rv:1.7.3) Gecko/20041001 Firefox/0.10.1");
		curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "gzip");// 设置压缩，保证编码正确
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT, useragent, URLAddress.c_str());

		//调用curl_easy_perform 执行我们的设置
		this->res = curl_easy_perform(this->curl);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
}

// 测试获取网页内容，然后写入到string缓存中
template<class T>
string RESTFulRequestor<T>::getGETResponse2Stream(string URLAddress) {
	// 设置当前获取的URL返回结果
	string readBuffer = "";

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, &RESTFulRequestor::WriteCallback2Stream);	// 设置回调函数来写入获取的数据
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer);			// 要写入的内容

		// 检查返回码，然后将获取的结果返回
		this->res = curl_easy_perform(this->curl);
		char *ct;
		this->res = curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ct);
		if (!(CURLE_OK == this->res) && !ct) {
			return "";
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return readBuffer;
}

// 访问指定URL，并且将返回的内容写入到文件中
template<class T>
bool RESTFulRequestor<T>::getGETResponse2File(string URLAddress, FILE *fptr) {
	// 设置返回值
	bool result = false;

	// 检查当前的文件是否正确
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 开始访问远程数据
	if (this->curl) {
		//设置访问代理
		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");

		//设置协议头
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //这是回调函数的第四个参数值

		// 执行
		this->res = curl_easy_perform(this->curl);

		// 检查当前访问网页的返回代码
		if (this->res != 0) {
			curl_slist_free_all(headers);
			result = false;
		}

		// 关闭文件流
		fclose(fptr);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}

// 访问指定URL，并且将返回的内容写入到文件中
template<class T>
bool RESTFulRequestor<T>::getGETResponseAsFile(string URLAddress, string fileName) {
	// 设置返回值
	bool result = false;

	// 初始化：
	this->curl = curl_easy_init();

	// 检查文件指针参数是否正确
	FILE *fp;
	if ((fp = fopen(fileName.c_str(), "w")) == NULL) {
		return false;
	}

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 开始访问远程数据
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fp); //将返回的http头输出到fp指向的文件
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, fp); //将返回的html主体数据输出到fp指向的文件
		
		// 执行
		this->res = curl_easy_perform(this->curl);

		// 检查当前访问网页的返回代码
		if (this->res != 0) {
			curl_slist_free_all(headers);
			result = false;
		}

		// 关闭文件流
		fclose(fp);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}




/**********************************************************************************/
/*********************************POST请求***************************************/
/**********************************************************************************/
// 构造POST请求，并且返回结果到文件中
template<class T>
string RESTFulRequestor<T>::getPOSTRequest2Stream(string URLAddress, string requestParameter) {
	// 设置当前获取的URL返回结果
	string readBuffer = "";

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 开始访问远程数据
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2Stream); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer); // 要写入的内容
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功

		// 执行POST请求：
		this->res = curl_easy_perform(this->curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			return NULL;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	// 将获取结果返回
	return readBuffer;
}

// 构造POST请求，并且返回结果记录在文件中
template<class T>
bool RESTFulRequestor<T>::getPOSTRequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	// 设置返回值
	bool result = false;

	// 检查当前的文件是否正确
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 开始访问远程数据
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //这是回调函数的第四个参数值
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功
		
		// 执行POST请求：
		this->res = curl_easy_perform(this->curl);
		
		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			return result;
		}
		else {
			result = true;
		}
		
		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	//
	return result;
}

// 构造POST请求，并且返回结果作为二进制文件存储
template<class T>
bool RESTFulRequestor<T>::getPOSTRequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	//
	bool result = false;
	// 构造回调参数
	struct DownloadFileStruct requestFileStruct = {
		//定义下载到本地的文件位置和路径
		localFilePath.c_str(), 
		NULL
	};

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 开始访问远程数据
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //这是回调函数的第四个参数值
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置为非0表示本次操作为post 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功

		// 执行POST请求：
		this->res = curl_easy_perform(this->curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			return false;
		}
		else {
			result = true;
		}

		// 关闭文件流：
		if (requestFileStruct.stream) {
			//关闭文件流
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	//
	return result;
}


/**********************************************************************************/
/**********************************DELETE请求************************************/
/**********************************************************************************/
// 发送DELETE请求，并且处理返回为字符串：
template<class T>
string RESTFulRequestor<T>::getDELETERequest2Stream(string URLAddress, string requestParameter) {
	// 设置当前获取的URL返回结果
	string readBuffer;

	// 初始化：
	this->curl = curl_easy_init();

	// 获取请求：
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// 设置为DELETE请求
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2Stream); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer); // 要写入的内容
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功

		 // 执行POST请求：
		this->res = curl_easy_perform(this->curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			return NULL;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	// 将获取结果返回
	return readBuffer;
}
// 发送DELETE请求，并且将返回结果写入到文件：
template<class T>
bool RESTFulRequestor<T>::getDELETERequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	bool result = false;
	// 检查当前的文件是否正确
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// 初始化：
	this->curl = curl_easy_init();

	// 获取请求：
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// 设置为DELETE请求
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //这是回调函数的第四个参数值
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功

		// 执行POST请求：
		this->res = curl_easy_perform(this->curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			result = false;
		}
		else {
			result = true;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	return result;
}
// 发送DELETE请求，并且将返回结果写入到指定文件：
template<class T>
bool RESTFulRequestor<T>::getDELETERequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	
	bool result = false;

	// 构造回调参数
	struct DownloadFileStruct requestFileStruct = {
		//定义下载到本地的文件位置和路径
		localFilePath.c_str(),
		NULL
	};

	// 初始化：
	this->curl = curl_easy_init();

	// 获取请求：
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// 设置为DELETE请求
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url地址 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post参数 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //这是回调函数的第四个参数值
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //设置为非0表示本次操作为post 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给回调函数 
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// 设置当前访问的cookie，否则不一定访问成功

		// 执行POST请求：
		this->res = curl_easy_perform(this->curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			result = false;
		} else {
			result = true;
		}

		// 关闭文件流：
		if (requestFileStruct.stream) {
			//关闭文件流
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	return result;
}




/**********************************************************************************/
/************************************PUT请求*************************************/
/**********************************************************************************/
// 发送PUT请求，并且处理返回：
template<class T>
string RESTFulRequestor<T>::getPUTRequest2Stream(string URLAddress, string requestParameter) {
	// 设置当前获取的URL返回结果
	string readBuffer;

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// 初始化：
	this->curl = curl_easy_init();
	// 如果初始化成功进入执行
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // 定制PUT请求，使用POST进行封装
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str());

		// 返回结果写入：
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, &RESTFulRequestor::WriteCallback2Stream);	// 设置回调函数来写入获取的数据
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer);// 要写入的内容

		// 检查返回码，然后将获取的结果返回
		this->res = curl_easy_perform(this->curl);
		if (CURLE_OK == this->res) {
			char *ct;
			this->res = curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ct);
			if (!(CURLE_OK == this->res) && !ct) {
				return NULL;
			}
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	return readBuffer;
}
// 发送PUT请求，并且处理返回：
template<class T>
bool RESTFulRequestor<T>::getPUTRequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	bool result = false;

	// 检查当前的文件是否正确
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // 定制PUT请求，使用POST进行封装
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); /* data goes here */
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //这是回调函数的第四个参数值

		// 检查返回码，然后将获取的结果返回
		this->res = curl_easy_perform(this->curl);
		if (CURLE_OK == this->res) {
			char *ct;
			this->res = curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ct);
			if (!(CURLE_OK == this->res) && !ct) {
				result = false;
			}
			result = true;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	return result;
}
// 发送PUT请求，并且处理返回：
template<class T>
bool RESTFulRequestor<T>::getPUTRequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	bool result = false;
	// 构造回调参数
	struct DownloadFileStruct requestFileStruct = {
		//定义下载到本地的文件位置和路径
		localFilePath.c_str(),
		NULL
	};

	// 初始化：
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // 定制PUT请求，使用POST进行封装
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); /* data goes here */
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //对返回的数据进行操作的函数地址
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //这是回调函数的第四个参数值

		// 检查返回码，然后将获取的结果返回
		this->res = curl_easy_perform(this->curl);
		if (CURLE_OK == this->res) {
			char *ct;
			this->res = curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ct);
			if (!(CURLE_OK == this->res) && !ct) {
				result = false;
			}
			result = true;
		}

		// 关闭文件流：
		if (requestFileStruct.stream) {
			//关闭文件流
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}


/**********************************************************************************/
/********************************FORM表单提交***********************************/
/**********************************************************************************/
/*
上传文件构造的表单结构为：
<form method="post" enctype="multipart/form-data">
<input id="filepath" name="filepath" type="file" />
<input type="submit" value="upload" />
<input name="act" type="hidden" value="upload" />
</form>
*/
// 上传本地文件到指定的URL
template<class T>
bool RESTFulRequestor<T>::uploadSimpleFormFile(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// 初始化：
	this->curl = curl_easy_init();

	// 设置表单需要的结构体
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	/* Fill in the file upload field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "filepath",
		CURLFORM_FILE, localFilePath.c_str(),
		CURLFORM_END);

	/* Fill in the filename field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "filename",
		CURLFORM_COPYCONTENTS, uploadFileName.c_str(),
		CURLFORM_END);

	/* Fill in the submit field too, even if this is rarely needed */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "submit",
		CURLFORM_COPYCONTENTS, "upload",
		CURLFORM_END);

	// initialize custom header list (stating that Expect: 100-continue is not wanted
	headerlist = curl_slist_append(headerlist, buf);

	// 准备执行：
	if (this->curl) {
		/* what URL that receives this POST */
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(this->curl, CURLOPT_HTTPPOST, formpost);

		// 执行提交
		this->res = curl_easy_perform(this->curl);

		// 检查是否成功
		if (this->res != CURLE_OK) {
			cout << "upload file error:" << curl_easy_strerror(this->res) << endl;
			result = false;
		}
		else {
			result = true;
		}

		// always cleanup
		curl_easy_cleanup(curl);

		// then cleanup the formpost chain
		curl_formfree(formpost);
		// free slist
		curl_slist_free_all(headerlist);
	}

	return result;
}

// 上传multipart结构的post文件：
template<class T>
bool RESTFulRequestor<T>::uploadMultipartFormFile(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// 初始化：
	this->curl = curl_easy_init();

	// 请求返回的数据 
	string readBuffer;

	// 设置表单需要的结构体
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	// 准备执行：
	if (this->curl) {
		// 设置
		curl_easy_setopt(curl, CURLOPT_URL, URLAddress.c_str());
		
		// 设置参数：
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
		
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, localFilePath.c_str(), CURLFORM_END);// 获取文件路径进行传输
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, uploadFileName.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "Submit", CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// 执行提交
		this->res = curl_easy_perform(this->curl);

		// 检查是否成功
		if (this->res != CURLE_OK) {
			cout << "upload file error:" << curl_easy_strerror(this->res) << endl;
			result = false;
		}
		else {
			result = true;
		}

		// always cleanup
		curl_easy_cleanup(curl);

		// then cleanup the formpost chain
		curl_formfree(formpost);
		// free slist
		curl_slist_free_all(headerlist);
	}

	// 最后返回结果
	return result;
}


// 上传multipart结构的post文件流：
template<class T>
bool RESTFulRequestor<T>::uploadMultipartFormFileStream(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// 初始化：
	this->curl = curl_easy_init();

	// 请求返回的数据 
	string readBuffer;

	// 设置表单需要的结构体
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	// 准备执行：
	if (this->curl) {
		// 设置
		curl_easy_setopt(curl, CURLOPT_URL, URLAddress.c_str());
		// 设置参数：
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);

		// 获取文件内容并且设置：
		FILE* file = fopen(localFilePath.c_str(), "r");
		fseek(file, 0, SEEK_END);
		long lSize = ftell(file);
		rewind(file);
		char *buffer = (char*)malloc(sizeof(char)*lSize);
		size_t result = fread(buffer, 1, lSize, file);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, lSize);
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, buffer, CURLFORM_CONTENTSLENGTH, lSize, CURLFORM_END);//将文件读入为流进行传输
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, localFilePath.c_str(), CURLFORM_END);

		// 设置基本文件信息：
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, uploadFileName.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "Submit", CURLFORM_END);

		// 设置post请求内容
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// 执行提交
		this->res = curl_easy_perform(this->curl);

		// 检查是否成功
		if (this->res != CURLE_OK) {
			cout << "upload file error:" << curl_easy_strerror(this->res) << endl;
			result = false;
		}
		else {
			result = true;
		}

		// always cleanup
		curl_easy_cleanup(curl);

		// then cleanup the formpost chain
		curl_formfree(formpost);
		// free slist
		curl_slist_free_all(headerlist);
	}

	// 最后返回结果
	return result;
}


/**********************************************************************************/
/**********************************下载请求***************************************/
/**********************************************************************************/
// 基于http的GET的同步下载文件方法
template<class T>
int RESTFulRequestor<T>::SyncDownload(string URLAddress, string requestParameter, string localFilePath) {
	// 初始化链接
	this->curl = curl_easy_init();

	// 获取当前文件的大小，进行比较
	FILE* fptr = fopen(localFilePath.c_str(), "ab+");
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}
	//  首先使用fseek将读写指针移动到文件尾部，然后获取当前已经存在的大小
	fseek(fptr, 0L, SEEK_END);
	unsigned long currentFilePosition = ftell(fptr);
	
	// 
	if (this->curl) {
		// 设置参数
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());//设置目的URL地址
		curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 100);//超时设置，超过设置时间会自动断开
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1);  //设置是否包含http头，如果包含http头会造成文件不正确，这儿需要去除
		//curl_easy_setopt(curl, CURLOPT_RANGE, "2-6"); //传递一个你想指定的范围下载指定字节的文件块
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File);//回调函数用此来保存接收到的数据点大小
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr);//函数会将接收到的数据自动的写到这个FILE指针所指向的文件流中。
		curl_easy_setopt(this->curl, CURLOPT_RESUME_FROM, currentFilePosition);  //从0字节开始下载（不是对比本地文件后的续传，而是从头开始再来）
		//curl_easy_setopt(this->curl, CURLOPT_CONNECTTIMEOUT, 10);  //设置下载超时10秒
		curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);//是否显示下载的详情：0表示显示，1表示不显示
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);


		// 执行下载
		this->res = curl_easy_perform(curl);

		// 返回状态检查：
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "不支持的协议,由URL的头部指定" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "不能连接到remote主机或者代理" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http返回错误" << endl;
			case CURLE_READ_ERROR:
				cout << "读本地文件错误" << endl;
			default:
				cout << "返回值：" << this->res << endl;
			}
			return false;
		}

		// 关闭文件流：
		if (fptr) {
			//关闭文件流
			fclose(fptr);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	//
	return this->res;
}


/**********************************************************************************/
/******************************JSON结构体的自动转换*****************************/
/**********************************************************************************/
// 反序列化操作：
template<class T>
bool RESTFulRequestor<T>::deserialize(T& instance, string jsonStr) {
	bool isSuccess = this->parser.deserialize(instance, jsonStr);
	return isSuccess;
}
// 序列化操作：
template<class T>
string RESTFulRequestor<T>::serialize(T a) {
	return this->parser.serialize(a);
}
