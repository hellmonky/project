// C++��װ�Ķ�RESTFul API������ʵ���ļ�
#include "RESTFulRequestor.h"

// JSONת���ӿ�
#include "AutoParser.cpp"

template<class T>
RESTFulRequestor<T>::RESTFulRequestor() {
	curl_global_init(CURL_GLOBAL_ALL);
	// ��ǰҪ���ʵ�URL��ַ
	this->URLAddress = "";
	// Ĭ��cookie�ļ�·��Ϊ��
	this->cookiePath = "";	
	// ����base64���볣��
	base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
								"abcdefghijklmnopqrstuvwxyz"
								"0123456789+/";
	// ��ӭ�´�
	std::cout << "welcome to use RESTFul API caller using C++ wrapper" << std::endl;
}

template<class T>
RESTFulRequestor<T>::~RESTFulRequestor() {
	// ���curl����.
	curl_global_cleanup();
	// �뿪�´�
	std::cout << "finish wrapper caller" << std::endl;
}


/**********************************************************************************/
/*********************************base64����*************************************/
/**********************************************************************************/
// base64����
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

// base64����
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
/*********************************�����ص�****************************************/
/**********************************************************************************/
// �ص�����������������д�뵽�ַ���������
template<class T>
size_t RESTFulRequestor<T>::WriteCallback2Stream(void* buffer, size_t size, size_t nmemb, void *stream) {
	((std::string*)stream)->append((char*)buffer, size * nmemb);
	return size * nmemb;
}

// �ص�����������������д�뵽�ļ�������
template<class T>
size_t RESTFulRequestor<T>::WriteCallback2File(void* buffer, size_t size, size_t nmemb, void *stream) {
	FILE *fptr = (FILE*)stream;

	//fwrite(buffer, size, nmemb, fptr);
	//return size*nmemb;

	size_t written = fwrite(buffer, size, nmemb, fptr);
	return written;
}

// �ص�����������ȡ�����ݰ��ն�������д�뵽�ļ��У��������ִ��ʱ��Ƚϳ�����ÿ�һ�º�̨�߳̽��д���
template<class T>
size_t RESTFulRequestor<T>::WriteCallbackAsFile(void* buffer, size_t size, size_t nmemb, void *stream) {
	struct DownloadFileStruct *out = (struct DownloadFileStruct *)stream;
	if (out && !out->stream) {
		// ���ն����Ƹ�ʽ���ļ�����д��
		out->stream = fopen(out->filename, "wb");
		if (!out->stream) {
			return -1;
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}


/**********************************************************************************/
/*********************************GET ����****************************************/
/**********************************************************************************/
// ����ָ����URL�����ҽ��������ݻ��Ե���Ļ��
template<class T>
void RESTFulRequestor<T>::getGETResponse2Screen(string URLAddress) {
	// ��ʼ����
	this->curl = curl_easy_init();
	
	//
	if (this->curl != NULL) {
		// init to NULL is important 
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");

		//���÷���URL��CURLOPT_URL
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());

		//���õ�ǰ���û�����Ϣ������α����������ʣ��е���վ��������Ŀ��ȷ���Ƿ��ṩ����
		struct curl_slist *useragent = NULL;
		useragent = curl_slist_append(useragent, "Mozilla/5.0 (Windows; U; Windows NT 5.1; rv:1.7.3) Gecko/20041001 Firefox/0.10.1");
		curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, "gzip");// ����ѹ������֤������ȷ
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT, useragent, URLAddress.c_str());

		//����curl_easy_perform ִ�����ǵ�����
		this->res = curl_easy_perform(this->curl);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
}

// ���Ի�ȡ��ҳ���ݣ�Ȼ��д�뵽string������
template<class T>
string RESTFulRequestor<T>::getGETResponse2Stream(string URLAddress) {
	// ���õ�ǰ��ȡ��URL���ؽ��
	string readBuffer = "";

	// ��ʼ����
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
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, &RESTFulRequestor::WriteCallback2Stream);	// ���ûص�������д���ȡ������
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer);			// Ҫд�������

		// ��鷵���룬Ȼ�󽫻�ȡ�Ľ������
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

// ����ָ��URL�����ҽ����ص�����д�뵽�ļ���
template<class T>
bool RESTFulRequestor<T>::getGETResponse2File(string URLAddress, FILE *fptr) {
	// ���÷���ֵ
	bool result = false;

	// ��鵱ǰ���ļ��Ƿ���ȷ
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����Զ������
	if (this->curl) {
		//���÷��ʴ���
		//curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");

		//����Э��ͷ
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //���ǻص������ĵ��ĸ�����ֵ

		// ִ��
		this->res = curl_easy_perform(this->curl);

		// ��鵱ǰ������ҳ�ķ��ش���
		if (this->res != 0) {
			curl_slist_free_all(headers);
			result = false;
		}

		// �ر��ļ���
		fclose(fptr);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}

// ����ָ��URL�����ҽ����ص�����д�뵽�ļ���
template<class T>
bool RESTFulRequestor<T>::getGETResponseAsFile(string URLAddress, string fileName) {
	// ���÷���ֵ
	bool result = false;

	// ��ʼ����
	this->curl = curl_easy_init();

	// ����ļ�ָ������Ƿ���ȷ
	FILE *fp;
	if ((fp = fopen(fileName.c_str(), "w")) == NULL) {
		return false;
	}

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����Զ������
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fp); //�����ص�httpͷ�����fpָ����ļ�
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, fp); //�����ص�html�������������fpָ����ļ�
		
		// ִ��
		this->res = curl_easy_perform(this->curl);

		// ��鵱ǰ������ҳ�ķ��ش���
		if (this->res != 0) {
			curl_slist_free_all(headers);
			result = false;
		}

		// �ر��ļ���
		fclose(fp);

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}




/**********************************************************************************/
/*********************************POST����***************************************/
/**********************************************************************************/
// ����POST���󣬲��ҷ��ؽ�����ļ���
template<class T>
string RESTFulRequestor<T>::getPOSTRequest2Stream(string URLAddress, string requestParameter) {
	// ���õ�ǰ��ȡ��URL���ؽ��
	string readBuffer = "";

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����Զ������
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2Stream); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer); // Ҫд�������
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //�����ʷ�0��ʾ���β���Ϊpost 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص�����
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�

		// ִ��POST����
		this->res = curl_easy_perform(this->curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
			}
			return NULL;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	// ����ȡ�������
	return readBuffer;
}

// ����POST���󣬲��ҷ��ؽ����¼���ļ���
template<class T>
bool RESTFulRequestor<T>::getPOSTRequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	// ���÷���ֵ
	bool result = false;

	// ��鵱ǰ���ļ��Ƿ���ȷ
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����Զ������
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //���ǻص������ĵ��ĸ�����ֵ
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //�����ʷ�0��ʾ���β���Ϊpost 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص�����
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�
		
		// ִ��POST����
		this->res = curl_easy_perform(this->curl);
		
		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
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

// ����POST���󣬲��ҷ��ؽ����Ϊ�������ļ��洢
template<class T>
bool RESTFulRequestor<T>::getPOSTRequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	//
	bool result = false;
	// ����ص�����
	struct DownloadFileStruct requestFileStruct = {
		//�������ص����ص��ļ�λ�ú�·��
		localFilePath.c_str(), 
		NULL
	};

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����Զ������
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //���ǻص������ĵ��ĸ�����ֵ
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //����Ϊ��0��ʾ���β���Ϊpost 
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص�����
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�

		// ִ��POST����
		this->res = curl_easy_perform(this->curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
			}
			return false;
		}
		else {
			result = true;
		}

		// �ر��ļ�����
		if (requestFileStruct.stream) {
			//�ر��ļ���
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	//
	return result;
}


/**********************************************************************************/
/**********************************DELETE����************************************/
/**********************************************************************************/
// ����DELETE���󣬲��Ҵ�����Ϊ�ַ�����
template<class T>
string RESTFulRequestor<T>::getDELETERequest2Stream(string URLAddress, string requestParameter) {
	// ���õ�ǰ��ȡ��URL���ؽ��
	string readBuffer;

	// ��ʼ����
	this->curl = curl_easy_init();

	// ��ȡ����
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// ����ΪDELETE����
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2Stream); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer); // Ҫд�������
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //�����ʷ�0��ʾ���β���Ϊpost 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص�����
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�

		 // ִ��POST����
		this->res = curl_easy_perform(this->curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
			}
			return NULL;
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	// ����ȡ�������
	return readBuffer;
}
// ����DELETE���󣬲��ҽ����ؽ��д�뵽�ļ���
template<class T>
bool RESTFulRequestor<T>::getDELETERequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	bool result = false;
	// ��鵱ǰ���ļ��Ƿ���ȷ
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// ��ʼ����
	this->curl = curl_easy_init();

	// ��ȡ����
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// ����ΪDELETE����
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //���ǻص������ĵ��ĸ�����ֵ
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //�����ʷ�0��ʾ���β���Ϊpost 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص�����
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�

		// ִ��POST����
		this->res = curl_easy_perform(this->curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
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
// ����DELETE���󣬲��ҽ����ؽ��д�뵽ָ���ļ���
template<class T>
bool RESTFulRequestor<T>::getDELETERequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	
	bool result = false;

	// ����ص�����
	struct DownloadFileStruct requestFileStruct = {
		//�������ص����ص��ļ�λ�ú�·��
		localFilePath.c_str(),
		NULL
	};

	// ��ʼ����
	this->curl = curl_easy_init();

	// ��ȡ����
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "DELETE");// ����ΪDELETE����
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str()); //url��ַ 
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); //post���� 
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //���ǻص������ĵ��ĸ�����ֵ
		curl_easy_setopt(this->curl, CURLOPT_POST, 1); //����Ϊ��0��ʾ���β���Ϊpost 
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1); //��ӡ������Ϣ 
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1); //����Ӧͷ��Ϣ����Ӧ��һ�𴫸��ص����� 
		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 1); //����Ϊ��0,��Ӧͷ��Ϣlocation 
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, this->cookiePath);// ���õ�ǰ���ʵ�cookie������һ�����ʳɹ�

		// ִ��POST����
		this->res = curl_easy_perform(this->curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
			}
			result = false;
		} else {
			result = true;
		}

		// �ر��ļ�����
		if (requestFileStruct.stream) {
			//�ر��ļ���
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}
	return result;
}




/**********************************************************************************/
/************************************PUT����*************************************/
/**********************************************************************************/
// ����PUT���󣬲��Ҵ����أ�
template<class T>
string RESTFulRequestor<T>::getPUTRequest2Stream(string URLAddress, string requestParameter) {
	// ���õ�ǰ��ȡ��URL���ؽ��
	string readBuffer;

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	// ��ʼ����
	this->curl = curl_easy_init();
	// �����ʼ���ɹ�����ִ��
	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // ����PUT����ʹ��POST���з�װ
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str());

		// ���ؽ��д�룺
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, &RESTFulRequestor::WriteCallback2Stream);	// ���ûص�������д���ȡ������
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &readBuffer);// Ҫд�������

		// ��鷵���룬Ȼ�󽫻�ȡ�Ľ������
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
// ����PUT���󣬲��Ҵ����أ�
template<class T>
bool RESTFulRequestor<T>::getPUTRequest2File(string URLAddress, string requestParameter, FILE *fptr) {
	bool result = false;

	// ��鵱ǰ���ļ��Ƿ���ȷ
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // ����PUT����ʹ��POST���з�װ
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); /* data goes here */
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr); //���ǻص������ĵ��ĸ�����ֵ

		// ��鷵���룬Ȼ�󽫻�ȡ�Ľ������
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
// ����PUT���󣬲��Ҵ����أ�
template<class T>
bool RESTFulRequestor<T>::getPUTRequestAsFile(string URLAddress, string requestParameter, string localFilePath) {
	bool result = false;
	// ����ص�����
	struct DownloadFileStruct requestFileStruct = {
		//�������ص����ص��ļ�λ�ú�·��
		localFilePath.c_str(),
		NULL
	};

	// ��ʼ����
	this->curl = curl_easy_init();

	// init to NULL is important 
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	if (this->curl) {
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "PUT"); // ����PUT����ʹ��POST���з�װ
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, requestParameter.c_str()); /* data goes here */
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallbackAsFile); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &requestFileStruct); //���ǻص������ĵ��ĸ�����ֵ

		// ��鷵���룬Ȼ�󽫻�ȡ�Ľ������
		this->res = curl_easy_perform(this->curl);
		if (CURLE_OK == this->res) {
			char *ct;
			this->res = curl_easy_getinfo(this->curl, CURLINFO_CONTENT_TYPE, &ct);
			if (!(CURLE_OK == this->res) && !ct) {
				result = false;
			}
			result = true;
		}

		// �ر��ļ�����
		if (requestFileStruct.stream) {
			//�ر��ļ���
			fclose(requestFileStruct.stream);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	return result;
}


/**********************************************************************************/
/********************************FORM���ύ***********************************/
/**********************************************************************************/
/*
�ϴ��ļ�����ı��ṹΪ��
<form method="post" enctype="multipart/form-data">
<input id="filepath" name="filepath" type="file" />
<input type="submit" value="upload" />
<input name="act" type="hidden" value="upload" />
</form>
*/
// �ϴ������ļ���ָ����URL
template<class T>
bool RESTFulRequestor<T>::uploadSimpleFormFile(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// ��ʼ����
	this->curl = curl_easy_init();

	// ���ñ���Ҫ�Ľṹ��
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

	// ׼��ִ�У�
	if (this->curl) {
		/* what URL that receives this POST */
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());
		curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(this->curl, CURLOPT_HTTPPOST, formpost);

		// ִ���ύ
		this->res = curl_easy_perform(this->curl);

		// ����Ƿ�ɹ�
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

// �ϴ�multipart�ṹ��post�ļ���
template<class T>
bool RESTFulRequestor<T>::uploadMultipartFormFile(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// ��ʼ����
	this->curl = curl_easy_init();

	// ���󷵻ص����� 
	string readBuffer;

	// ���ñ���Ҫ�Ľṹ��
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	// ׼��ִ�У�
	if (this->curl) {
		// ����
		curl_easy_setopt(curl, CURLOPT_URL, URLAddress.c_str());
		
		// ���ò�����
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
		
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, localFilePath.c_str(), CURLFORM_END);// ��ȡ�ļ�·�����д���
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, uploadFileName.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "Submit", CURLFORM_END);

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// ִ���ύ
		this->res = curl_easy_perform(this->curl);

		// ����Ƿ�ɹ�
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

	// ��󷵻ؽ��
	return result;
}


// �ϴ�multipart�ṹ��post�ļ�����
template<class T>
bool RESTFulRequestor<T>::uploadMultipartFormFileStream(string URLAddress, string uploadFileName, string localFilePath) {
	bool result = false;

	// ��ʼ����
	this->curl = curl_easy_init();

	// ���󷵻ص����� 
	string readBuffer;

	// ���ñ���Ҫ�Ľṹ��
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	// ׼��ִ�У�
	if (this->curl) {
		// ����
		curl_easy_setopt(curl, CURLOPT_URL, URLAddress.c_str());
		// ���ò�����
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);

		// ��ȡ�ļ����ݲ������ã�
		FILE* file = fopen(localFilePath.c_str(), "r");
		fseek(file, 0, SEEK_END);
		long lSize = ftell(file);
		rewind(file);
		char *buffer = (char*)malloc(sizeof(char)*lSize);
		size_t result = fread(buffer, 1, lSize, file);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buffer);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, lSize);
		curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, buffer, CURLFORM_CONTENTSLENGTH, lSize, CURLFORM_END);//���ļ�����Ϊ�����д���
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, localFilePath.c_str(), CURLFORM_END);

		// ���û����ļ���Ϣ��
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, uploadFileName.c_str(), CURLFORM_END);
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "Submit", CURLFORM_END);

		// ����post��������
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		// ִ���ύ
		this->res = curl_easy_perform(this->curl);

		// ����Ƿ�ɹ�
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

	// ��󷵻ؽ��
	return result;
}


/**********************************************************************************/
/**********************************��������***************************************/
/**********************************************************************************/
// ����http��GET��ͬ�������ļ�����
template<class T>
int RESTFulRequestor<T>::SyncDownload(string URLAddress, string requestParameter, string localFilePath) {
	// ��ʼ������
	this->curl = curl_easy_init();

	// ��ȡ��ǰ�ļ��Ĵ�С�����бȽ�
	FILE* fptr = fopen(localFilePath.c_str(), "ab+");
	if (NULL == fptr) {
		cout << "File pointer error" << endl;
		return false;
	}
	//  ����ʹ��fseek����дָ���ƶ����ļ�β����Ȼ���ȡ��ǰ�Ѿ����ڵĴ�С
	fseek(fptr, 0L, SEEK_END);
	unsigned long currentFilePosition = ftell(fptr);
	
	// 
	if (this->curl) {
		// ���ò���
		curl_easy_setopt(this->curl, CURLOPT_URL, URLAddress.c_str());//����Ŀ��URL��ַ
		curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 100);//��ʱ���ã���������ʱ����Զ��Ͽ�
		//curl_easy_setopt(this->curl, CURLOPT_HEADER, 1);  //�����Ƿ����httpͷ���������httpͷ������ļ�����ȷ�������Ҫȥ��
		//curl_easy_setopt(curl, CURLOPT_RANGE, "2-6"); //����һ������ָ���ķ�Χ����ָ���ֽڵ��ļ���
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback2File);//�ص������ô���������յ������ݵ��С
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, fptr);//�����Ὣ���յ��������Զ���д�����FILEָ����ָ����ļ����С�
		curl_easy_setopt(this->curl, CURLOPT_RESUME_FROM, currentFilePosition);  //��0�ֽڿ�ʼ���أ����ǶԱȱ����ļ�������������Ǵ�ͷ��ʼ������
		//curl_easy_setopt(this->curl, CURLOPT_CONNECTTIMEOUT, 10);  //�������س�ʱ10��
		curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);//�Ƿ���ʾ���ص����飺0��ʾ��ʾ��1��ʾ����ʾ
		//curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);


		// ִ������
		this->res = curl_easy_perform(curl);

		// ����״̬��飺
		if (this->res != CURLE_OK) {
			switch (this->res) {
			case CURLE_UNSUPPORTED_PROTOCOL:
				cout << "��֧�ֵ�Э��,��URL��ͷ��ָ��" << endl;
			case CURLE_COULDNT_CONNECT:
				cout << "�������ӵ�remote�������ߴ���" << endl;
			case CURLE_HTTP_RETURNED_ERROR:
				cout << "http���ش���" << endl;
			case CURLE_READ_ERROR:
				cout << "�������ļ�����" << endl;
			default:
				cout << "����ֵ��" << this->res << endl;
			}
			return false;
		}

		// �ر��ļ�����
		if (fptr) {
			//�ر��ļ���
			fclose(fptr);
		}

		// always cleanup
		curl_easy_cleanup(this->curl);
	}

	//
	return this->res;
}


/**********************************************************************************/
/******************************JSON�ṹ����Զ�ת��*****************************/
/**********************************************************************************/
// �����л�������
template<class T>
bool RESTFulRequestor<T>::deserialize(T& instance, string jsonStr) {
	bool isSuccess = this->parser.deserialize(instance, jsonStr);
	return isSuccess;
}
// ���л�������
template<class T>
string RESTFulRequestor<T>::serialize(T a) {
	return this->parser.serialize(a);
}
