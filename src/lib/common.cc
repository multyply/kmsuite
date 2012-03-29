#include "common.h"

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <sys/types.h>
#include <unistd.h>

using namespace std;

void trim_left(string& data,const string& space=" \t\n\r") {
	size_t pos=data.find_first_not_of(space);
	data.erase(0,pos);
}

void trim_right(string& data,const string& space=" \t\n\r") {
	size_t pos=data.find_last_not_of(space);
	data.erase(pos+1);
}

void trim(string& data,const string& space=" \t\n\r") {
	trim_left(data,space);
	trim_right(data,space);
}

bool isDigit(const string str) {
	for ( unsigned int i = 0; i < str.size(); i++ ) {
		if ( str[i] < '0' || str [i] > '9') return false;
	}

	return true;
}

string replaceFirst(string str, string from, string to) {
	string result;
	int idx = str.find(from);
	if (idx == -1 ) return str;

	result = str.substr(0, idx);
	result += to;
	result += str.substr(idx+from.size());

	return result;
}

string replace(string str, string from, string to) {
	string result = str;
	string tmp;
	int idx = 0;
	while ( (idx = result.find(from, idx)) != -1 ) {
		tmp = result.substr(0, idx);
		tmp += to;
		tmp += result.substr(idx+from.size());

		result = tmp;
		idx += to.size();
	}

	return result;
}

int getRandomInt() {
	int i;
	FILE *file;

	file = fopen("/dev/urandom", "r");
	fread(&i, 1, sizeof(int), file);
	fclose(file);

	return i;
}

bool isSafeCharacter(char chr) {
	return (
		(chr >= 32 && chr <= 126)
//		||
//		(chr >= 128 && chr <= 255)
	);
}

string toURLDecode(string str) {
	char buf[3];
	int c;
	string dec;
	
	int len = str.size();
	int i;
	for(i=0; i<len; i++) {
		switch(str[i]) {
			case '%':
				if (i>=len-2) return string();  /* return en empty string */

				buf[0] = str[++i];
				buf[1] = str[++i];
				buf[2] = '\0';
				sscanf(buf, "%2x", &c);
				break;
			case '+':
				c = ' ';
				break;
			default:
				c = str[i];
		}

		dec += (char)c;

	}

	return dec;
}

string toURLEncode(string str) {
	return string(toURLDecode(str.c_str()));
}

string toString(const int t) {
	stringstream ss;
	ss << t;
	return ss.str();
}

int stringToCRC(char *str) {
	int c = 0;
	for (unsigned int i=0; i<strlen(str); i++) {
		c += (int)str[i];
	}

	return c;
}

int toInteger(const string str) {
	stringstream ss(str);
	int i;

	ss >> i;

	return i;
}

string toUpper(const string str) {
	string tmp = str;

	for (unsigned int i=0; i<str.size(); i++) {
		tmp[i] = toupper(tmp[i]);
	}

	return tmp;
}

string toLower(const string str) {
	string tmp = str;

	for (unsigned int i=0; i<str.size(); i++) {
		tmp[i] = tolower(tmp[i]);
	}

	return tmp;
}

string getHome() {
#ifdef WIN32
	cerr << "Not implementet yet." << endl;
	exit(2);
#endif
	string file;
	file += "/proc/";
//	file += toString(getpid());
	file += "self";
	file += "/exe";

	char buf[4096];                             /* get 4kb of RAM */
	int len = readlink(file.c_str(), buf, sizeof(buf)-1);
	if ( len == -1 ) {
		cerr << "Unable to get home-path." << endl;
		exit(1);
	}

	buf[len] = '\0';
	string path = buf;
	int idx = path.rfind("/bin/");
	if (idx == -1 ) {
		cerr << "Invalid home-path." << endl;
		exit(1);
	}

	path = path.substr(0, idx);

	return path;
}
