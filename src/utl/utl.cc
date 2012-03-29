#include "utl.h"

#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

using namespace std;

void copyright() {
	cout << endl << PRG_NAME << " v." << PRG_VERSION << endl;
	cout << "(c) " << PRG_DATE << " by " << PRG_AUTHOR << endl << endl;
}

void usage(const string program) {
	cout	<< program << " MODULENAME" << endl;
}

bool isFile(const string filename) {
	struct stat st;
	return ( stat(filename.c_str(), &st) == 0 );
}

string getModuleName(const string module) {
	if (module.size()<1) {
		cerr << "ERROR: Module-name is emtpy" << endl;
		exit(1);
	}

	string tmp = module;
	tmp[0] = toupper(tmp[0]);

	return tmp;
}

string correctString(const string str) {
	string tmp = str;

	tmp = replace(tmp, "\\", "\\\\");
	tmp = replace(tmp, "\"", "\\\"");
	tmp = replace(tmp, "%", "%%");

//	trim(tmp, " \t\n\r");

	return tmp;
}

string getViewFilename(const string module) {
	string filename = CONFIG_WEB_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".html";

	return filename;
}

string getViewHeaderFilename(const string module) {
	string filename = CONFIG_SOURCE_DIRECTORY;
	filename += "/";
	filename += CONFIG_MODULE_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".web.h";

	return filename;
}

string getViewControllerFilename(const string module) {
	string filename = CONFIG_SOURCE_DIRECTORY;
	filename += "/";
	filename += CONFIG_MODULE_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".web.cc";

	return filename;
}

string getControllerFilename(const string module) {
	string filename = CONFIG_SOURCE_DIRECTORY;
	filename += "/";
	filename += CONFIG_MODULE_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".cc";

	return filename;
}

string getBinaryFilename(const string module) {
	string filename = CONFIG_BINARY_DIRECTORY;
	filename += "/";
	filename += CONFIG_MODULE_DIRECTORY;
	filename += "/";
	filename += module;

	return filename;
}

string getMakeFilename(const string module) {
	return "Makefile";
}
