
#ifndef COMMON_H
#define COMMON_H

#include <string>

using namespace std;

void trim_left(string& data,const string&);
void trim_right(string& data,const string&);
void trim(string& data,const string&);
bool isDigit(const string str);
string replaceFirst(string str, string from, string to);
string replace(string str, string from, string to);
int getRandomInt();
//char *toURLDecode(const char *str);
//char *toURLEncode(const char *str);
string toURLDecode(string str);
string toURLEncode(string str);
string toString (const int t);
int toInteger (const string t);
int stringToCRC(char *str);
string toUpper(const string str);
string toLower(const string str);
string getHome();

#endif
