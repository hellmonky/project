// RESTFul API ���ʽӿ�
#include "RESTFulRequestor.cpp"

/*********************************************************************/
// ���Խṹ������л��ͷ����л�
struct Person {
	int age;
	string name;
	string city;

	// import : to add meteinfo to autoparser
	META(age, name, city)
};
Person p = { 18, "bb", "aa" };

/*********************************************************************/
// ����Ƕ�׽ṹ��ת������
struct Pos_OUT
{
	float lat;//γ��
	float lon;//����
	float atl;//�߶ȣ�����û��

	// metaInfo
	META(lat, lon, atl)
};
struct Action_OUT
{
	string id;//ʵ��id����һ����
	string name;//·���������ߵص�����XXX��
	string code;//·����ص���
	string startTime;//��ʼʱ��
	string stopTime;//����ʱ��
	vector<Pos_OUT> posList;//;��λ�õ㣬���Ϊ�ص㣬���׶�2����ֻ��һ��

	// metaInfo
	META(id, name, code, startTime, stopTime, posList)
};
struct Object_OUT
{
	string id;
	string name;//����
	string code;//���
	int  type;//����

	// metaInfo
	META(id, name, code, type)
};
struct Plan_OUT
{
	string name;//��������
	string startTime;//��ʼʱ��
	string stopTime;//����ʱ��
	int stage;//�׶�
	vector<Object_OUT> objList;//�ж�Ŀ������
	vector<Action_OUT> rounteList;//�ж�����

	// metaInfo
	META(name, startTime, stopTime, stage, objList, rounteList)
};
struct DataContent {
	int type;
	vector<Plan_OUT> command;

	// metaInfo
	META(type, command)
};
struct Content {
	string timestamp;
	vector<DataContent> data;

	// metaInfo
	META(timestamp, data)
};
struct Mission {
	string from;
	string to;
	int type;
	Content content;

	// metaInfo
	META(from, to, type, content)
};

/*********************************************************************/
// ����ת��������
void complexStrcutTest() {
	//
	Pos_OUT post_out_1 = { double(18.9), double(20.5), double(0) };
	Pos_OUT post_out_2 = { float(19.9), float(21.5), float(0) };
	Pos_OUT post_out_3 = { float(20.9), float(22.5), float(0) };
	vector<Pos_OUT> posList;
	//
	posList.push_back(post_out_1);
	posList.push_back(post_out_2);
	posList.push_back(post_out_3);
	//
	Action_OUT action_out = { "1", "����333", "001", "20161019100000", "20161019110000", posList };
	//
	Object_OUT object_out_1 = { "1", "Ŀ��1", "001", 1 };
	Object_OUT object_out_2 = { "2", "Ŀ��2", "002", 1 };
	//
	vector<Object_OUT> objList;
	objList.push_back(object_out_1);
	objList.push_back(object_out_2);
	vector<Action_OUT> rounteList;
	rounteList.push_back(action_out);
	Plan_OUT plan_out = { "����1�׶�1", "20161019080000", "20161021080000", 1, objList, rounteList };
	//
	vector<Plan_OUT> command;
	command.push_back(plan_out);
	DataContent datacontent = { 1, command };
	//
	vector<DataContent> data;
	data.push_back(datacontent);
	Content content = { "20161019103030", data };
	//
	Mission mission = { "sourceId", "targetId", 1, content };


	// ��ʼ���л�ΪJSON��
	RESTFulRequestor<Mission> restfulK;
	// ���ԣ����Զ���ṹ��ת��ΪJSON
	string serializitionResult = restfulK.serialize(mission);
	cout << "ת�����JSON���Ϊ��" << serializitionResult << endl;
	// ���ԣ���JSONת�����Զ���ṹ��
	Mission deserializitionResult;
	bool result = restfulK.deserialize(deserializitionResult, serializitionResult);
	if (result) {
		cout << "�����л��ɹ���" << endl;
	}
}




// �ַ���ת������
std::string utf8_from_iso8859_1(std::string str)
{
	std::string res;
	for (std::string::iterator i = str.begin(); i < str.end(); i++) {
		if (0 <= *i && *i < 0x80)
			res += *i;
		else {
			res += 0xC0 | ((*i >> 6) & 0x03);
			res += 0x80 | (*i & 0x3F);
		}
	}
	return res;
}
string GBKToUTF8(const std::string & strGBK) {
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

string UTF8ToGBK(const std::string & strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}


// ���Ժ�����ڣ�
int main(int argc, char * argv[]) {

	//
	complexStrcutTest();

	// ��ʼ��restful�������ʵ��
	RESTFulRequestor<Person> request;

	/**********************************************************************************/
	// ����������ԣ�
	string GetChineseURL = "http://172.16.10.14:1569/datasong/dataService/test/student/0EfwrIIfQUc9FuPbQNTeYx20161014113922976";
	string GetChineseResult = request.getGETResponse2Stream(GetChineseURL);
	cout << UTF8ToGBK(GetChineseResult) << endl;
	//string fileName = "C:\\Users\\wentao\\Desktop\\out.txt";
	//bool isGetSuccess = request.getGETResponseAsFile(GetChineseURL, fileName);
	
	// PUT�������ò��ԣ�
	string PutURL = "http://172.16.10.14:1569/datasong/dataService/iscas/mission";
	string PutParamters = "{\"_actionid\" : \"1\", \"_id\" : \"\",  \"_name\" : \"����1\",  \"_remark\" : \"�ҵĵ�һ������\",  \"_time\" : 2.119999999999999}";
	cout << PutParamters << endl;
	string PutResult = request.getPUTRequest2Stream(PutURL, GBKToUTF8(PutParamters));
	cout << UTF8ToGBK(PutResult) << endl;
	//string PutFileName = "C:\\Users\\wentao\\Desktop\\out.txt";
	//bool isPUTSuccess = request.getPUTRequestAsFile(GetChineseURL, PutParamters, PutFileName);

	// POST�������ò��ԣ�
	string POSTURL = "http://172.16.10.14:1569/datasong/dataService/test/mission";
	string POSTParamters = "{\"search\":{\"field\":\"_actionid\",\"NAME\":\"term\",\"values\":[\"1\"]},\"start\":0,\"size\":10} ";
	string POSTResult = request.getPOSTRequest2Stream(PutURL, PutParamters);
	cout << UTF8ToGBK(POSTResult) << endl;


	/**********************************************************************************/
	// ���ԣ����Զ���ṹ��ת��ΪJSON
	string serializitionResult = request.serialize(p);
	cout << "ת�����JSON���Ϊ��" << serializitionResult << endl;
	// ���ԣ���JSONת�����Զ���ṹ��
	Person deserializitionResult;
	bool result = request.deserialize(deserializitionResult, serializitionResult);
	if (result) {
		cout << "�����л��ɹ���" << endl;
		cout << deserializitionResult.age << endl;
		cout << deserializitionResult.name << endl;
		cout << deserializitionResult.city << endl;
	}

	/**********************************************************************************/
	// �ۺϲ�����ڣ�
	if (argc<3) {	// ������������
		cout << "at least input 2 paramters" << endl;
	}
	// ��ȡ������
	int testOption = atoi(argv[1]);
	cout << "current option is: "<< testOption << endl;

	// �����������ִ�в��ԣ�
	switch (testOption)
	{
	// ����0��������ַ�����ҽ���ҳ����������ʾ����Ļ��
	case 0: {
		if (argc != 3) {
			cout << "call method: exe 0 URLAddress" << endl;
			break;
		}
		string testURL = argv[2];
		cout << "����GET���������ַ��" << testURL << endl;
		cout << "���ؽ��Ϊ��" << endl;
		request.getGETResponse2Screen(testURL);
		break;
	}
	// ����1��ʹ��GET��������URL�����������Ϊ�ַ�������
	case 1: {
		if (argc != 3) {
			cout << "call method: exe 1 URLAddress" << endl;
			break;
		}
		string testURL = argv[2];
		cout << "ʹ��GET��������RESTFul�ӿڣ�" << testURL << endl;
		string result = request.getGETResponse2Stream(testURL);
		cout << "���ؽ��Ϊ��" << UTF8ToGBK(result) << endl;
		break;
	}
	// ����2��ʹ��GET��������URL���������д��ָ�����ļ���
	case 2: {
		if (argc != 4) {
			cout << "call method: exe 2 URLAddress LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string restorFilePath = argv[3];
		cout << "ʹ��GET��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "�����ؽ��д��ָ�����ļ���" << restorFilePath << endl;
		bool result = request.getGETResponseAsFile(testURL, restorFilePath);
		if (result) {
			cout << "���ؽ��д���ļ��ɹ�" <<endl;
		}
		else {
			cout << "���ؽ��д���ļ�ʧ��" << endl;
		}
		break;
	}
	// ����3��ʹ��POST��������URL�����������Ϊ�ַ�������
	case 3: {
		if (argc != 4) {
			cout << "call method: exe 3 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// ִ�в�����
		cout << "ʹ��POST��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ���Ϊ��" << requestParameter << endl;
		string buffer = request.getPOSTRequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "��ȡ��������ʧ��" << endl;
		}
		else {
			cout << "���ؽ��Ϊ:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// ����4��ʹ��POST��������URL���������д�뵽ָ�����ļ���
	case 4: {
		if (argc != 5) {
			cout << "call method: exe 4 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// ִ�в�����
		cout << "ʹ��GET��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ�����" << requestParameter << endl;
		cout << "�����ؽ��д��ָ�����ļ���" << restorFilePath << endl;
		bool result = request.getPOSTRequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "���ؽ��д���ļ��ɹ�" << endl;
		}
		else {
			cout << "���ؽ��д���ļ�ʧ��" << endl;
		}
		break;
	}
	// ����5��ʹ��DELETE��������URL�����������Ϊ�ַ�������
	case 5: {
		if (argc != 4) {
			cout << "call method: exe 5 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// ִ�в�����
		cout << "ʹ��DELETE��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ���Ϊ��" << requestParameter << endl;
		string buffer = request.getDELETERequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "��ȡ��������ʧ��" << endl;
		}
		else {
			cout << "���ؽ��Ϊ:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// ����6��ʹ��DELETE��������URL���������д�뵽ָ�����ļ���
	case 6: {
		if (argc != 5) {
			cout << "call method: exe 6 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// ִ�в�����
		cout << "ʹ��DELETE��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ�����" << requestParameter << endl;
		cout << "�����ؽ��д��ָ�����ļ���" << restorFilePath << endl;
		bool result = request.getDELETERequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "���ؽ��д���ļ��ɹ�" << endl;
		}
		else {
			cout << "���ؽ��д���ļ�ʧ��" << endl;
		}
		break;
	}
	// ����7��ʹ��PUT��������URL�����������Ϊ�ַ�������
	case 7: {
		if (argc != 4) {
			cout << "call method: exe 7 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// ִ�в�����
		cout << "ʹ��DELETE��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ���Ϊ��" << requestParameter << endl;
		string buffer = request.getPUTRequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "��ȡ��������ʧ��" << endl;
		}
		else {
			cout << "���ؽ��Ϊ:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// ����8��ʹ��DELETE��������URL���������д�뵽ָ�����ļ���
	case 8: {
		if (argc != 5) {
			cout << "call method: exe 8 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// ִ�в�����
		cout << "ʹ��PUT��������RESTFul�ӿڣ�" << testURL << endl;
		cout << "ָ���Ĳ�����" << requestParameter << endl;
		cout << "�����ؽ��д��ָ�����ļ���" << restorFilePath << endl;
		bool result = request.getPUTRequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "���ؽ��д���ļ��ɹ�" << endl;
		}
		else {
			cout << "���ؽ��д���ļ�ʧ��" << endl;
		}
		break;
	}
	// ����9��form���ύ�����ļ���ָ����URL
	case 9: {
		if (argc != 5) {
			cout << "call method: exe 9 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// ִ�в�����
		cout << "form���ύ�����ļ���" << localFilePath << endl;
		cout << "���ļ���Ϊ��" << uploadFileName << endl;
		cout << "�ύ��ָ����URL��" << testURL << endl;
		bool result = request.uploadSimpleFormFile(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "ִ�гɹ������ڷ������˲鿴�ϴ��ļ�" << endl;
		}
		break;
	}
	// ����10��form����multipart��ʽ�ύ�����ļ���ָ����URL
	case 10: {
		if (argc != 5) {
			cout << "call method: exe 10 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// ִ�в�����
		cout << "form����multipart��ʽ�ύ�����ļ���" << localFilePath << endl;
		cout << "���ļ���Ϊ��" << uploadFileName << endl;
		cout << "�ύ��ָ����URL��" << testURL << endl;
		bool result = request.uploadMultipartFormFile(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "ִ�гɹ������ڷ������˲鿴�ϴ��ļ�" << endl;
		}
		break;
	}
	// ����11��form����multipart��ʽ�������ļ�ת��Ϊ�ļ�����Ȼ���ύ��ָ����URL
	case 11: {
		if (argc != 5) {
			cout << "call method: exe 11 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// ִ�в�����
		cout << "form����multipart��ʽ�������ļ���" << localFilePath <<" ת��Ϊ�ļ���"<< endl;
		cout << "���ļ���Ϊ��" << uploadFileName << endl;
		cout << "�ύ��ָ����URL��" << testURL << endl;
		bool result = request.uploadMultipartFormFileStream(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "ִ�гɹ������ڷ������˲鿴�ϴ��ļ�" << endl;
		}
		break;
	}
	// ����12����ָ����URL�����ļ�������·����
	case 12: {
		if (argc != 5) {
			cout << "call method: exe 12 URLAddress requestParameter localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string localFilePath = argv[4];
		// ִ�в�����
		cout << "��ָ����URL��" << testURL << endl;
		cout << "���ղ�����" << requestParameter << endl;
		cout << "�����ļ�������·����" << localFilePath << endl;
		int result = request.SyncDownload(testURL, requestParameter, localFilePath);
		if (result == 0) {
			cout << "ִ�гɹ������ڱ���·���鿴�����ļ�" << endl;
		}
		break;
	}
	default:
		break;
	}


	// Finish test
	return 0;
}