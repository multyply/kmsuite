#include "parameter.h"

#include "common.h"

#include <iostream>

using namespace std;

Parameter::Parameter() {
}

Parameter::~Parameter() {
}

void Parameter::add(string key, string value) {
	if (!list[key].empty()) list.erase(key);    /* remove old entry */
	list[key] = value;
}

void Parameter::add(string key, int value) {
	add(key, toString(value)); 
}

string Parameter::get(string key) {
	return list[key];
}

string Parameter::getKey(int index) {
	if (index < size()) {
		map<string, string>::iterator it = list.begin();
		for (int i=0; i<index; i++) {
			it++;
		}
		return it->first;
	} else return string();
}

string Parameter::getValue(int index) {
	if (index < size()) {
		map<string, string>::iterator it = list.begin();
		for (int i=0; i<index; i++) {
			it++;
		}
		return it->second;
	} else return string();
}

int Parameter::getInteger(string key) {
	string value = get(key);
	return value.empty() ? -1 : toInteger(value);
}

void Parameter::del(string key) {
	list.erase(key);
}

void Parameter::delAll() {
	list.clear();
}

int Parameter::size() {
	return static_cast<int>(list.size());
}
