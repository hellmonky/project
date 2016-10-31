#include "AutoParser.h"

template<class T>
AutoParser<T>::AutoParser() {
	cout << "welcome to auto parser to JSON-Struct" << endl;
}

template<class T>
AutoParser<T>::~AutoParser() {
	cout << "goodbye" << endl;
}

template<class T>
bool AutoParser<T>::deserialize(T& instance, string jsonStr) {
	this->dr.Parse(jsonStr);
	this->dr.Deserialize(instance);
	return true;
}

template<class T>
string AutoParser<T>::serialize(T a) {
	this->sr.Serialize(a);
	return this->sr.GetString();
}