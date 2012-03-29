/*
 * CHTML Project
 *
 * Create - checks and creates the directory structure and files
 */

#include "utl.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

void createView(const string module);
void createHeader(const string module);
void createController(const string module);
void createMake(const string module);

int main(int argc, char *argv[]) {

	copyright();

	if ( argc < 2 ) {
		cout << "ERROR:\t\tInvalid arguments." << endl;
		usage(string(argv[0]));
		exit(1);
	}

	string module = argv[1];
	cout << "MODULE:\t\t" << getModuleName(module) << endl;

	createView(module);
	createHeader(module);
	createController(module);
	createMake(module);

	return(0);
}

void createView(const string module) {
	string filename = getViewFilename(module);
	cout << "VIEW:\t\t" << filename << endl;

	if (isFile(filename)) {
		cerr << "WARNING:\tView already exists (" << filename << ")." << endl;
		return;
	}

	fstream file;
	file.open(filename.c_str(), fstream::out);
	if ( !file ) {
		cerr << "ERROR:\t\tUnable to open view-file (" << filename << ")." << endl;
		exit(1);
	}

	file << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"" << endl;
	file << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << endl;
	file << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
	file << "\t<head>" << endl;
	file << "\t\t<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/>" << endl;
	file << "\t\t<meta http-equiv=\"content-script-type\" content=\"text/javascript\"/>" << endl;
	file << "\t\t<meta name=\"robots\" content=\"index, follow\"/>" << endl;
	file << "\t\t<meta http-equiv=\"pragma\" content=\"no-cache\"/>" << endl;
	file << "\t\t<meta http-equiv=\"imagetoolbar\" content=\"no\"/>" << endl;
	file << "\t\t<meta name=\"generator\" content=\"Symano\"/>" << endl;
	file << "\t\t<meta name=\"author\" content=\"\"/>" << endl;
	file << "\t\t<meta name=\"company\" content=\"\"/>" << endl;
	file << "\t\t<meta name=\"description\" content=\"\"/>" << endl;
	file << "\t\t<meta name=\"keywords\" content=\"\"/>" << endl;
	file << "\t\t<script src=\"/script/default.js\" type=\"text/javascript\"></script>" << endl;
	file << "\t\t<link href=\"/style/default.css\" type=\"text/css\" rel=\"stylesheet\"/>" << endl;
	file << "\t\t<title><%=title%></title>" << endl;
	file << "\t</head>" << endl;
	file << "\t<body>" << endl;
	file << "\t\t<%" << endl;
	file << "\t\t\tprintf ( \"<b>This is C++!</b>\\n\" );" << endl;
	file << "\t\t%>" << endl;
	file << "\t</body>" << endl;
	file << "<html>" << endl;

	file.close();
}

void createHeader(const string module) {
	string filename = getViewHeaderFilename(module);
	cout << "VIEW-HEADER:\t" << filename << endl;

	if (isFile(filename)) {
		cerr << "WARNING:\tHeader already exists (" << filename << ")." << endl;
		return;
	}

	fstream file;
	file.open(filename.c_str(), fstream::out);
	if ( !file ) {
		cerr << "ERROR:\t\tUnable to open header-file (" << filename << ")." << endl;
		exit(1);
	}

	file << "#ifndef MODULE_" << toUpper(module) << "_H" << endl;
	file << "#define MODULE_" << toUpper(module) << "_H" << endl << endl;
	file << "#include \"chtml.lib.h\"" << endl << endl;
	file << "class " << getModuleName(module) << "Module : public CHTML {" << endl;
	file << "\tpublic:" << endl;
	file << "\t\tvoid init();" << endl;
	file << "\t\tvoid destroy();" << endl;
	file << "\t\tvoid service();" << endl;
	file << "\t\tvoid printWeb();" << endl;
	file << "};" << endl << endl;
	file << "#endif" << endl;

	file.close();
}

void createController(const string module) {
	string filename = getControllerFilename(module);
	cout << "CONTROLLER:\t" << filename << endl;

	if (isFile(filename)) {
		cerr << "WARNING:\tController already exists (" << filename << ")." << endl;
		return;
	}

	fstream file;
	file.open(filename.c_str(), fstream::out);
	if ( !file ) {
		cerr << "ERROR:\t\tUnable to open controller-file (" << filename << ")." << endl;
		exit(1);
	}

	file << "#include \"" << module << ".web.h\"" << endl << endl;
	file << "void " << getModuleName(module) << "Module::service() {" << endl;
//	file << "\trequest->parseGet();" << endl << endl;
	file << "\tresponse->addParameter(\"title\", \"" << getModuleName(module) << "\");" << endl;
	file << "}" << endl;

	file.close();
}

void createMake(const string module) {
	string filename = getMakeFilename(module);
	cout << "MAKE:\t\t" << filename << endl;

	stringstream buffer(stringstream::in | stringstream::out);
	fstream file;

	/* read Makefile */
	file.open(filename.c_str(), ios::in);
	if (!file) {
		cerr << "ERROR\t\tUnable to read Makefile." << endl;
		exit(1);
	}

	string line;
	while (file.good()) {
		getline(file, line);
		if (line.find("mod:") == 0) {
			if (((int)line.find("mod_" + module)) > 15) {
				cerr << "WARNING:\tModule already exists." << endl;
				file.close();
				return;
			} else {
				buffer << line << " mod_" << module << endl;
			}
		} else {
			buffer << line << endl;
		}
	}

	file.close();

	buffer << "mod_" << module << ":" << endl;
	buffer << "\tbin/utl/compile " << module << " > /dev/null" << endl;
	buffer << "\t$(CC) $(LDFLAGS) -o bin/mod/" << module << ".chtml src/mod/" << module << ".cc src/mod/" << module << ".web.cc bin/lib/chtml.o bin/lib/chtml.a";

	/* write Makefile */
	file.open(filename.c_str(), ios::out);
	if (!file) {
		cerr << "ERROR\t\tUnable to write Makefile." << endl;
		exit(1);
	}

	file << buffer.str();

	file.close();
}
