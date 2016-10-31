// C++封装的对RESTFul API调用类头文件
#pragma once

#ifndef RESTFULREQUEST_H
#define RESTFULREQUEST_H


// standar library
#include <string>
#include <iostream>
#include <fstream>

// 3rd library
#include "curl\curl.h"
#include "curl\easy.h"

// JSON转换接口
#include "AutoParser.h"

// 使用标准命名空间
using namespace std;

// class define
template<class T> class RESTFulRequestor {
	
public:
	RESTFulRequestor();		// 默认构造函数
	~RESTFulRequestor();		// 默认析构函数

// 请求相关：
public:
	// 获取GET请求并且返回参数：
	void getGETResponse2Screen(string URLAddress);// 为了测试
	string getGETResponse2Stream(string URLAddress);
	bool getGETResponse2File(string URLAddress, FILE *fptr);
	bool getGETResponseAsFile(string URLAddress, string fileName);

	// 获取POST请求，并且处理返回：
	string getPOSTRequest2Stream(string URLAddress, string requestParameter);
	bool getPOSTRequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getPOSTRequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// 发送DELETE请求，并且处理返回：
	string getDELETERequest2Stream(string URLAddress, string requestParameter);
	bool getDELETERequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getDELETERequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// 发送PUT请求，并且处理返回：
	string getPUTRequest2Stream(string URLAddress, string requestParameter);
	bool getPUTRequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getPUTRequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// 构造form表单的文件上传：
	bool uploadSimpleFormFile(string URLAddress, string uploadFileName, string localFilePath);				// 正常form表单文件提交
	bool uploadMultipartFormFile(string URLAddress, string uploadFileName, string localFilePath);			// multipartFile类型form表单提交
	bool uploadMultipartFormFileStream(string URLAddress, string uploadFileName, string localFilePath);	// multipartFile类型文件流的form表单提交

	// 下载函数：支持断点续传，需要服务器也支持断点续传；用于网络环境不稳定的断点续传下载（没有加入文件效验，之针针对当前给定文件路径确认）
	int SyncDownload(string URLAddress, string requestParameter, string localFilePath);


// 请求相关：
private:
	string URLAddress;		// 当前要访问的URL地址
	CURL *curl;					// 定义CURL类型的指针
	CURLM *multi_handle;// 定义多线程指针
	CURLcode res;				// 定义CURLcode类型的变量，保存返回状态码
	string cookiePath;		// 设置访问http服务器的本地cookie
	string base64_chars;	// 用于base64加密和解密
	
	// 使用结构体对回调函数传参
	struct DownloadFileStruct {
		const char *filename;
		FILE *stream;
	};

	// 判断当前字符是否是base64规定的字符
	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	};
	string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	string base64_decode(std::string const& encoded_string);

	// curl用于获取数据的回掉处理函数
	static size_t WriteCallback2Stream(void* buffer, size_t size, size_t nmemb, void *stream);
	// curl获取数据并且写入到文件中
	static size_t WriteCallback2File(void* buffer, size_t size, size_t nmemb, void *stream);	
	// curl获取http协议的文件数据
	static size_t WriteCallbackAsFile(void *buffer, size_t size, size_t nmemb, void *stream);

// JSON解析相关：
public:
	//JsonCppUtil<T> jsonParser;
	bool deserialize(T& instance, string jsonStr);
	string serialize(T a);
private:
	AutoParser<T> parser;


// 字符转换：
};

#endif

