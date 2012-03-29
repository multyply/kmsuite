/*
 * CHTML Project
 *
 * Drop - removes the complete module from the project
 */

#include "utl.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

using namespace std;

void dropView(const string module);
void dropViewHeader(const string module);
void dropViewController(const string module);
void dropController(const string module);
void dropBinary(const string module);
void dropMake(const string module);

int main(int argc, char *argv[]) {

	copyright();

	if ( argc < 2 ) {
		cout << "ERROR:\t\tInvalid arguments." << endl;
		usage(string(argv[0]));
		exit(1);
	}

	string module = argv[1];
	cout << "MODULE:\t\t" << getModuleName(module) << endl;

	dropView(module);
	dropViewHeader(module);
	dropViewController(module);
	dropController(module);
	dropBinary(module);
	dropMake(module);

	return(0);
}

void dropView(const string module) {
	string filename = getViewFilename(module);
	cout << "VIEW:\t\t" << filename << endl;
	
	if (!isFile(filename)) {
		cerr << "WARNING:\tView doesn't exists (" << filename << ")." << endl;
		return;
	}

	if (remove(filename.c_str())) {
		cerr << "ERROR:\t\tUnable to delete view (" << filename << ")." << endl;
		exit(1);
	}
}

void dropViewHeader(const string module) {
	string filename = getViewHeaderFilename(module);
	cout << "VIEW-HEADER:\t" << filename << endl;

	if (!isFile(filename)) {
		cerr << "WARNING:\tHeader doesn't exists (" << filename << ")." << endl;
		return;
	}

	if (remove(filename.c_str())) {
		cerr << "ERROR:\t\tUnable to delete header (" << filename << ")." << endl;
		exit(1);
	}
}
void dropViewController(const string module) {
	string filename = getViewControllerFilename(module);
	cout << "VIEW-CONTROLLER:" << filename << endl;

	if (!isFile(filename)) {
		cerr << "WARNING:\tController doesn't exists (" << filename << ")." << endl;
		return;
	}

	if (remove(filename.c_str())) {
		cerr << "ERROR:\t\tUnable to delete controller (" << filename << ")." << endl;
		exit(1);
	}
}

void dropController(const string module) {
	string filename = getControllerFilename(module);
	cout << "CONTROLLER:\t" << filename << endl;

	if (!isFile(filename)) {
		cerr << "WARNING:\tController doesn't exists (" << filename << ")." << endl;
		return;
	}

	if (remove(filename.c_str())) {
		cerr << "ERROR:\t\tUnable to delete controller (" << filename << ")." << endl;
		exit(1);
	}
}

void dropBinary(const string module) {
	string filename = getBinaryFilename(module);
	cout << "BINARY:\t\t" << filename << endl;

	if (!isFile(filename)) {
		cerr << "WARNING:\tBinary doesn't exists (" << filename << ")." << endl;
		return;
	}

	if (remove(filename.c_str())) {
		cerr << "ERROR:\t\tUnable to delete binary (" << filename << ")." << endl;
		exit(1);
	}
}

void dropMake(const string module) {
	string filename = getMakeFilename(module);
	cout << "MAKE:\t\t" << filename << endl;

	if (!isFile(filename)) {
		cerr << "ERROR:\tMake doesn't exists (" << filename << ")." << endl;
		exit(1);
	}

	stringstream buffer(stringstream::in | stringstream::out);

	fstream file;

	/* read Makefile */
	file.open(filename.c_str(), ios::in);
	if (!file) {
		cerr << "ERROR\t\tUnable to read Makefile." << endl;
		exit(1);
	}

	string line;
	bool remove = false;
	while (file.good()) {
		getline(file, line);
		if (line.find("mod:") == 0) {
			int idx = line.find("mod_" + module + " ");
			/* check if the module is the last in the line */
			if (idx == -1) {
				idx = line.find("mod_" + module);
				if (idx != -1) {
					if (idx+4+module.size() != line.size()) idx = -1;
				}
			}
			if (idx > 15) {
				buffer << line.substr(0,idx-1) << line.substr(idx+4+module.size()) << endl;
				continue;
			} else {
				cerr << "ERROR:\t\tModule doesn't exists." <<  endl;
				file.close();
				return;
			}
		}
		if (((int)line.find("mod_" + module + ":")) == 0) {
			remove = true;
			continue;
		}

		if (((int)line.find("mod_")) == 0) {
			remove = false;
		}

		if (remove) continue;

		buffer << line << endl;
	}

	file.close();


	/* write Makefile */
	file.open(filename.c_str(), ios::out);
	if (!file) {
		cerr << "ERROR\t\tUnable to write Makefile." << endl;
		exit(1);
	}

	file << buffer.str();

	file.close();
}
