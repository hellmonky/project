// C++��װ�Ķ�RESTFul API������ͷ�ļ�
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

// JSONת���ӿ�
#include "AutoParser.h"

// ʹ�ñ�׼�����ռ�
using namespace std;

// class define
template<class T> class RESTFulRequestor {
	
public:
	RESTFulRequestor();		// Ĭ�Ϲ��캯��
	~RESTFulRequestor();		// Ĭ����������

// ������أ�
public:
	// ��ȡGET�����ҷ��ز�����
	void getGETResponse2Screen(string URLAddress);// Ϊ�˲���
	string getGETResponse2Stream(string URLAddress);
	bool getGETResponse2File(string URLAddress, FILE *fptr);
	bool getGETResponseAsFile(string URLAddress, string fileName);

	// ��ȡPOST���󣬲��Ҵ����أ�
	string getPOSTRequest2Stream(string URLAddress, string requestParameter);
	bool getPOSTRequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getPOSTRequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// ����DELETE���󣬲��Ҵ����أ�
	string getDELETERequest2Stream(string URLAddress, string requestParameter);
	bool getDELETERequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getDELETERequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// ����PUT���󣬲��Ҵ����أ�
	string getPUTRequest2Stream(string URLAddress, string requestParameter);
	bool getPUTRequest2File(string URLAddress, string requestParameter, FILE *fptr);
	bool getPUTRequestAsFile(string URLAddress, string requestParameter, string localFilePath);

	// ����form�����ļ��ϴ���
	bool uploadSimpleFormFile(string URLAddress, string uploadFileName, string localFilePath);				// ����form���ļ��ύ
	bool uploadMultipartFormFile(string URLAddress, string uploadFileName, string localFilePath);			// multipartFile����form���ύ
	bool uploadMultipartFormFileStream(string URLAddress, string uploadFileName, string localFilePath);	// multipartFile�����ļ�����form���ύ

	// ���غ�����֧�ֶϵ���������Ҫ������Ҳ֧�ֶϵ��������������绷�����ȶ��Ķϵ��������أ�û�м����ļ�Ч�飬֮����Ե�ǰ�����ļ�·��ȷ�ϣ�
	int SyncDownload(string URLAddress, string requestParameter, string localFilePath);


// ������أ�
private:
	string URLAddress;		// ��ǰҪ���ʵ�URL��ַ
	CURL *curl;					// ����CURL���͵�ָ��
	CURLM *multi_handle;// ������߳�ָ��
	CURLcode res;				// ����CURLcode���͵ı��������淵��״̬��
	string cookiePath;		// ���÷���http�������ı���cookie
	string base64_chars;	// ����base64���ܺͽ���
	
	// ʹ�ýṹ��Իص���������
	struct DownloadFileStruct {
		const char *filename;
		FILE *stream;
	};

	// �жϵ�ǰ�ַ��Ƿ���base64�涨���ַ�
	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	};
	string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	string base64_decode(std::string const& encoded_string);

	// curl���ڻ�ȡ���ݵĻص�������
	static size_t WriteCallback2Stream(void* buffer, size_t size, size_t nmemb, void *stream);
	// curl��ȡ���ݲ���д�뵽�ļ���
	static size_t WriteCallback2File(void* buffer, size_t size, size_t nmemb, void *stream);	
	// curl��ȡhttpЭ����ļ�����
	static size_t WriteCallbackAsFile(void *buffer, size_t size, size_t nmemb, void *stream);

// JSON������أ�
public:
	//JsonCppUtil<T> jsonParser;
	bool deserialize(T& instance, string jsonStr);
	string serialize(T a);
private:
	AutoParser<T> parser;


// �ַ�ת����
};

#endif

