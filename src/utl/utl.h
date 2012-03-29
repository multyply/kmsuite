#ifndef UTL_H
#define UTL_H

#define PRG_NAME "CHTML Project"
#define PRG_VERSION "0.4"
#define PRG_DATE "2012"
#define PRG_AUTHOR "Stephan Laukien"

#define TPL_LINE 2048

#include "common.h"
#include "config.h"
#include <string>

using namespace std;

void copyright();
void usage(const string program);
bool isFile(const string filename);
string getModuleName(const string module);
string correctString(const string str);
string getViewFilename(const string module);
string getViewHeaderFilename(const string module);
string getViewControllerFilename(const string module);
string getControllerFilename(const string module);
string getBinaryFilename(const string module);
string getMakeFilename(const string module);

#endif
