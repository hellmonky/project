// ʹ��Kapok��Ϊ���л��ͷ����л���֧��
#pragma once

#ifndef AUTOPARSER_H
#define AUTOPARSER_H

// standar library
#include <string>
#include <iostream>
#include <fstream>

// 3rd library
# include "kapok\Kapok.hpp"

using namespace std;

template<class T> class AutoParser {
public:
	AutoParser();
	~AutoParser();
	bool deserialize(T& instance, string jsonStr);
	string serialize(T a);
private:
	Serializer sr;
	DeSerializer dr;
};

// for template link
//# include "AutoParser.cpp"
#endif