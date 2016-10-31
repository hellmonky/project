// RESTFul API 访问接口
#include "RESTFulRequestor.cpp"

/*********************************************************************/
// 测试结构体的序列化和反序列化
struct Person {
	int age;
	string name;
	string city;

	// import : to add meteinfo to autoparser
	META(age, name, city)
};
Person p = { 18, "bb", "aa" };

/*********************************************************************/
// 复杂嵌套结构体转换测试
struct Pos_OUT
{
	float lat;//纬度
	float lon;//经度
	float atl;//高度，可能没有

	// metaInfo
	META(lat, lon, atl)
};
struct Action_OUT
{
	string id;//实体id，不一定有
	string name;//路线名，或者地点名，XXX县
	string code;//路径或地点编号
	string startTime;//开始时间
	string stopTime;//到达时间
	vector<Pos_OUT> posList;//途经位置点，如果为地点，即阶段2，则只有一项

	// metaInfo
	META(id, name, code, startTime, stopTime, posList)
};
struct Object_OUT
{
	string id;
	string name;//名称
	string code;//编号
	int  type;//类型

	// metaInfo
	META(id, name, code, type)
};
struct Plan_OUT
{
	string name;//方案名称
	string startTime;//开始时间
	string stopTime;//到达时间
	int stage;//阶段
	vector<Object_OUT> objList;//行动目标序列
	vector<Action_OUT> rounteList;//行动序列

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
// 测试转换函数：
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
	Action_OUT action_out = { "1", "国道333", "001", "20161019100000", "20161019110000", posList };
	//
	Object_OUT object_out_1 = { "1", "目标1", "001", 1 };
	Object_OUT object_out_2 = { "2", "目标2", "002", 1 };
	//
	vector<Object_OUT> objList;
	objList.push_back(object_out_1);
	objList.push_back(object_out_2);
	vector<Action_OUT> rounteList;
	rounteList.push_back(action_out);
	Plan_OUT plan_out = { "任务1阶段1", "20161019080000", "20161021080000", 1, objList, rounteList };
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


	// 开始序列化为JSON：
	RESTFulRequestor<Mission> restfulK;
	// 测试：从自定义结构体转换为JSON
	string serializitionResult = restfulK.serialize(mission);
	cout << "转换后的JSON结果为：" << serializitionResult << endl;
	// 测试：从JSON转换到自定义结构体
	Mission deserializitionResult;
	bool result = restfulK.deserialize(deserializitionResult, serializitionResult);
	if (result) {
		cout << "反序列化成功！" << endl;
	}
}




// 字符集转换函数
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


// 测试函数入口：
int main(int argc, char * argv[]) {

	//
	complexStrcutTest();

	// 初始化restful请求类的实例
	RESTFulRequestor<Person> request;

	/**********************************************************************************/
	// 中文乱码测试：
	string GetChineseURL = "http://172.16.10.14:1569/datasong/dataService/test/student/0EfwrIIfQUc9FuPbQNTeYx20161014113922976";
	string GetChineseResult = request.getGETResponse2Stream(GetChineseURL);
	cout << UTF8ToGBK(GetChineseResult) << endl;
	//string fileName = "C:\\Users\\wentao\\Desktop\\out.txt";
	//bool isGetSuccess = request.getGETResponseAsFile(GetChineseURL, fileName);
	
	// PUT方法调用测试：
	string PutURL = "http://172.16.10.14:1569/datasong/dataService/iscas/mission";
	string PutParamters = "{\"_actionid\" : \"1\", \"_id\" : \"\",  \"_name\" : \"任务1\",  \"_remark\" : \"我的第一个任务\",  \"_time\" : 2.119999999999999}";
	cout << PutParamters << endl;
	string PutResult = request.getPUTRequest2Stream(PutURL, GBKToUTF8(PutParamters));
	cout << UTF8ToGBK(PutResult) << endl;
	//string PutFileName = "C:\\Users\\wentao\\Desktop\\out.txt";
	//bool isPUTSuccess = request.getPUTRequestAsFile(GetChineseURL, PutParamters, PutFileName);

	// POST方法调用测试：
	string POSTURL = "http://172.16.10.14:1569/datasong/dataService/test/mission";
	string POSTParamters = "{\"search\":{\"field\":\"_actionid\",\"NAME\":\"term\",\"values\":[\"1\"]},\"start\":0,\"size\":10} ";
	string POSTResult = request.getPOSTRequest2Stream(PutURL, PutParamters);
	cout << UTF8ToGBK(POSTResult) << endl;


	/**********************************************************************************/
	// 测试：从自定义结构体转换为JSON
	string serializitionResult = request.serialize(p);
	cout << "转换后的JSON结果为：" << serializitionResult << endl;
	// 测试：从JSON转换到自定义结构体
	Person deserializitionResult;
	bool result = request.deserialize(deserializitionResult, serializitionResult);
	if (result) {
		cout << "反序列化成功！" << endl;
		cout << deserializitionResult.age << endl;
		cout << deserializitionResult.name << endl;
		cout << deserializitionResult.city << endl;
	}

	/**********************************************************************************/
	// 综合测试入口：
	if (argc<3) {	// 检查参数个数：
		cout << "at least input 2 paramters" << endl;
	}
	// 获取参数：
	int testOption = atoi(argv[1]);
	cout << "current option is: "<< testOption << endl;

	// 根据输入参数执行测试：
	switch (testOption)
	{
	// 测试0：访问网址，并且将网页返回内容显示在屏幕上
	case 0: {
		if (argc != 3) {
			cout << "call method: exe 0 URLAddress" << endl;
			break;
		}
		string testURL = argv[2];
		cout << "按照GET请求访问网址：" << testURL << endl;
		cout << "返回结果为：" << endl;
		request.getGETResponse2Screen(testURL);
		break;
	}
	// 测试1：使用GET方法访问URL，并将结果作为字符串返回
	case 1: {
		if (argc != 3) {
			cout << "call method: exe 1 URLAddress" << endl;
			break;
		}
		string testURL = argv[2];
		cout << "使用GET方法请求RESTFul接口：" << testURL << endl;
		string result = request.getGETResponse2Stream(testURL);
		cout << "返回结果为：" << UTF8ToGBK(result) << endl;
		break;
	}
	// 测试2：使用GET方法访问URL，并将结果写入指定的文件中
	case 2: {
		if (argc != 4) {
			cout << "call method: exe 2 URLAddress LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string restorFilePath = argv[3];
		cout << "使用GET方法请求RESTFul接口：" << testURL << endl;
		cout << "将返回结果写入指定的文件：" << restorFilePath << endl;
		bool result = request.getGETResponseAsFile(testURL, restorFilePath);
		if (result) {
			cout << "返回结果写入文件成功" <<endl;
		}
		else {
			cout << "返回结果写入文件失败" << endl;
		}
		break;
	}
	// 测试3：使用POST方法访问URL，并将结果作为字符串返回
	case 3: {
		if (argc != 4) {
			cout << "call method: exe 3 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// 执行操作：
		cout << "使用POST方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数为：" << requestParameter << endl;
		string buffer = request.getPOSTRequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "获取网络请求失败" << endl;
		}
		else {
			cout << "返回结果为:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// 测试4：使用POST方法访问URL，并将结果写入到指定的文件中
	case 4: {
		if (argc != 5) {
			cout << "call method: exe 4 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// 执行操作：
		cout << "使用GET方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数：" << requestParameter << endl;
		cout << "将返回结果写入指定的文件：" << restorFilePath << endl;
		bool result = request.getPOSTRequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "返回结果写入文件成功" << endl;
		}
		else {
			cout << "返回结果写入文件失败" << endl;
		}
		break;
	}
	// 测试5：使用DELETE方法访问URL，并将结果作为字符串返回
	case 5: {
		if (argc != 4) {
			cout << "call method: exe 5 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// 执行操作：
		cout << "使用DELETE方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数为：" << requestParameter << endl;
		string buffer = request.getDELETERequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "获取网络请求失败" << endl;
		}
		else {
			cout << "返回结果为:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// 测试6：使用DELETE方法访问URL，并将结果写入到指定的文件中
	case 6: {
		if (argc != 5) {
			cout << "call method: exe 6 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// 执行操作：
		cout << "使用DELETE方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数：" << requestParameter << endl;
		cout << "将返回结果写入指定的文件：" << restorFilePath << endl;
		bool result = request.getDELETERequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "返回结果写入文件成功" << endl;
		}
		else {
			cout << "返回结果写入文件失败" << endl;
		}
		break;
	}
	// 测试7：使用PUT方法访问URL，并将结果作为字符串返回
	case 7: {
		if (argc != 4) {
			cout << "call method: exe 7 URLAddress requestParameter" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		// 执行操作：
		cout << "使用DELETE方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数为：" << requestParameter << endl;
		string buffer = request.getPUTRequest2Stream(testURL, requestParameter);
		if (buffer.empty()) {
			cout << "获取网络请求失败" << endl;
		}
		else {
			cout << "返回结果为:" << endl;
			cout << UTF8ToGBK(buffer) << endl;
		}
		break;
	}
	// 测试8：使用DELETE方法访问URL，并将结果写入到指定的文件中
	case 8: {
		if (argc != 5) {
			cout << "call method: exe 8 URLAddress requestParameter LocalFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string restorFilePath = argv[4];
		// 执行操作：
		cout << "使用PUT方法请求RESTFul接口：" << testURL << endl;
		cout << "指定的参数：" << requestParameter << endl;
		cout << "将返回结果写入指定的文件：" << restorFilePath << endl;
		bool result = request.getPUTRequestAsFile(testURL, requestParameter, restorFilePath);
		if (result) {
			cout << "返回结果写入文件成功" << endl;
		}
		else {
			cout << "返回结果写入文件失败" << endl;
		}
		break;
	}
	// 测试9：form表单提交本地文件到指定的URL
	case 9: {
		if (argc != 5) {
			cout << "call method: exe 9 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// 执行操作：
		cout << "form表单提交本地文件：" << localFilePath << endl;
		cout << "以文件名为：" << uploadFileName << endl;
		cout << "提交到指定的URL：" << testURL << endl;
		bool result = request.uploadSimpleFormFile(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "执行成功，请在服务器端查看上传文件" << endl;
		}
		break;
	}
	// 测试10：form表单以multipart方式提交本地文件到指定的URL
	case 10: {
		if (argc != 5) {
			cout << "call method: exe 10 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// 执行操作：
		cout << "form表单以multipart格式提交本地文件：" << localFilePath << endl;
		cout << "以文件名为：" << uploadFileName << endl;
		cout << "提交到指定的URL：" << testURL << endl;
		bool result = request.uploadMultipartFormFile(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "执行成功，请在服务器端查看上传文件" << endl;
		}
		break;
	}
	// 测试11：form表单以multipart方式将本地文件转换为文件流，然后提交到指定的URL
	case 11: {
		if (argc != 5) {
			cout << "call method: exe 11 URLAddress uploadFileName localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string uploadFileName = argv[3];
		string localFilePath = argv[4];
		// 执行操作：
		cout << "form表单以multipart方式将本地文件：" << localFilePath <<" 转换为文件流"<< endl;
		cout << "以文件名为：" << uploadFileName << endl;
		cout << "提交到指定的URL：" << testURL << endl;
		bool result = request.uploadMultipartFormFileStream(testURL, uploadFileName, localFilePath);
		if (result) {
			cout << "执行成功，请在服务器端查看上传文件" << endl;
		}
		break;
	}
	// 测试12：从指定的URL下载文件到本地路径：
	case 12: {
		if (argc != 5) {
			cout << "call method: exe 12 URLAddress requestParameter localFilePath" << endl;
			break;
		}
		string testURL = argv[2];
		string requestParameter = argv[3];
		string localFilePath = argv[4];
		// 执行操作：
		cout << "从指定的URL：" << testURL << endl;
		cout << "按照参数：" << requestParameter << endl;
		cout << "下载文件到本地路径：" << localFilePath << endl;
		int result = request.SyncDownload(testURL, requestParameter, localFilePath);
		if (result == 0) {
			cout << "执行成功，请在本地路径查看下载文件" << endl;
		}
		break;
	}
	default:
		break;
	}


	// Finish test
	return 0;
}