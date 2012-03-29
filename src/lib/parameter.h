
#ifndef PARAMETER_H
#define PARAMETER_H

#include <map>
#include <string>

using namespace std;

class Parameter {
	private:
	map<string, string> list;
	public:
	Parameter();
	~Parameter();
	void add(string key, string value);
	void add(string key, int value);
	string get(string key);
	string getKey(int index);
	string getValue(int index);
	int getInteger(string key);
	void del(string key);
	void delAll();
	int size();
};

#endif
