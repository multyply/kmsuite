/*
 * CHTML Project
 *
 * Compile - compiles templates to C-code
 */
#include "utl.h"
#include "config.h"
#include "common.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

bool isMethod(const string module);
void analyzeView(const string module);
void createViewController(const string module);
void cleanView(const string module);
string directiveInclude(string parameter);

int main(int argc, char *argv[]) {

	copyright();

	if ( argc < 2 ) {
		cout << "ERROR:\t\tInvalid arguments." << endl;
		usage(string(argv[0]));
		exit(1);
	}

	string module = argv[1];
	cout << "MODULE:\t\t" << getModuleName(module) << endl;

	analyzeView(module);
	createViewController(module);
	cleanView(module);

	return(0);
}

void analyzeView(const string module) {
	string filename_in = getViewFilename(module);
	string filename_out = filename_in + ".tmp";

	fstream file_in;
	file_in.open(filename_in.c_str(), ios::in);
	if (!file_in) {
		cerr << "ERROR\t\tUnable to read View." << endl;
		exit(1);
	}

	fstream file_out;
	file_out.open(filename_out.c_str(), ios::out);

	string line;
	int idx_start;
	int idx_end;
	int count = 0;
	while (file_in.good()) {
		count++;
		getline(file_in, line);
		idx_start = line.find("<%@");
		if (idx_start != -1) {
			idx_end = line.find("%>", idx_start);
			if (idx_end == -1) {
				cerr << "ERROR:\t\tBroken directive-tag in line " << count << "." << endl;
				exit(1);
			}

			string directive = line.substr(idx_start+3, idx_end-idx_start-3);
			trim(directive, " \t\r\n");
			idx_end = directive.find(" ");
			if (idx_end == -1) {
				cerr << "ERROR:\t\tInvalid directive-tag (" << directive << ") in line " << count << "." << endl;
				exit(1);
			}

			string command = directive.substr(0, idx_end);
			string parameter = directive.substr(idx_end);
			trim(parameter, " \t\n\r");

			if (command == "include") {
				line = directiveInclude(parameter);
			} else {
				cerr << "ERROR:\t\tUnknown directive (" << directive << ") in line " << count << "." << endl;
				exit(1);
			}
		}

		file_out << line << endl;
	}

	file_out.close();
	file_in.close();
}

bool isMethod(const string module, const string method) {
	ifstream ctl;
	ctl.open(getControllerFilename(module).c_str());
	if (ctl.is_open()) {
		string name = getModuleName(module) + "Module";
		string line;
		size_t pos;
		size_t cmt;
		while (ctl.good()) {
			getline(ctl, line);
			
			// init
			pos = line.find("void " + name + "::" + method + "()");
			if (pos != string::npos) {
				//check for comment
				cmt = line.find("//");
				if (cmt == string::npos) cmt = line.find("/*");
				if (cmt == string::npos) {
					ctl.close();
					return true;
				} else {
					// check for comment AFTER init
					if (cmt > pos) {
						ctl.close();
						return true;
					}
				}
			}
		}

		ctl.close();
	}

	return false;
}

void createViewController(const string module) {
	/* get info about 'init' & 'destroy' */
	
	bool isInit = isMethod(module, "init");
	bool isDestroy = isMethod(module, "destroy");

	/* module.web.cc */
	string filename = getViewControllerFilename(module);
	
	cout << "SOURCE:\t\t" << filename << endl;

	fstream file;
	file.open(filename.c_str(), fstream::out);
	if ( !file ) {
		cerr << "ERROR:\t\tUnable to open source-file (" << filename << ")." << endl;
		exit(1);
	}

	/* header */
	file << "#include \"" << module << ".web.h\"" << endl << endl;
	file << "#include <cstdio>" << endl;
	file << "#include <iostream>" << endl << endl;

	/* main */
	file << "int main() {" << endl;
	file << "\t" << getModuleName(module) << "Module *" << module << "_mod = new " << getModuleName(module) << "Module();" << endl << endl;
	if (isInit) file << "\t" << module << "_mod->init();" << endl << endl;
	file << "\t" << module << "_mod->request->parseQuery();" << endl << endl;
	file << "\t" << module << "_mod->service();" << endl << endl;
	file << "\t" << module << "_mod->response->printHeader();" << endl;
	file << "\t" << module << "_mod->printWeb();" << endl << endl;
	if (isDestroy) file << "\t" << module << "_mod->destroy();" << endl << endl;
	file << "\tdelete " << module << "_mod;" << endl << endl;
	file << "\treturn(0);" << endl;
	file << "}" << endl << endl;
	
	/* printTemplate */
	file << "void " << getModuleName(module) << "Module::printWeb() {" << endl;
//    file << "\tprintf ( \"\\n\" );" << endl;

	string filename_tmp = getViewFilename(module);
	filename_tmp += ".tmp";


	fstream file_tmp;
	file_tmp.open(filename_tmp.c_str(), fstream::in);
	if ( !file_tmp ) {
		cerr << "ERROR: Unable to open chtml-file (" << filename_tmp << ")." << endl;
		exit(1);
	}


//	string tdata((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	/* parse line by line */
	bool isCode = false;
	char line[TPL_LINE+1];
	int count = 0;
	int idx_start, idx_stop;
	string tmp;
	while(file_tmp.getline(line, TPL_LINE)) {
		count++;
		string str = line;
		trim(str, " \t\n\r");
		if ( str.size() == 0 ) continue;

		idx_start = str.find("<%");
		idx_stop = (idx_start == -1) ? str.find("%>") : str.find("%>", idx_start);


		if ( idx_start != -1 ) {
			tmp = str.substr(0, idx_start);
			tmp = correctString(tmp);

			/* template-code */
			if ( tmp.size() > 0 ) file << "\tprintf ( \"" << tmp << "\" );" << endl;

			/* C-code */
			if ( idx_stop == -1 ) {
				isCode = true;
				tmp = str.substr(idx_start+2);
				tmp = correctString(tmp);

				/* template-code */
				if ( tmp.size() > 0 ) file << "\t" << tmp << endl;
			} else {
				tmp = str.substr(idx_start+2, idx_stop-idx_start-2);
				trim(tmp, " \t\n\r");

				if ( tmp.size() > 0 ) {
					if ( tmp[0] == '=' ) {
						tmp = tmp.substr(1);
						trim(tmp, " ");
						file << "\tprintf ( \"%s\", response->getParameter(\"" << tmp << "\").c_str() );" << endl;
					} else {
						file << "\t" << tmp << endl;
					}
				}
				tmp = str.substr(idx_stop+2);
				tmp = correctString(tmp);
				if ( tmp.size() > 0 ) file << "\tprintf ( \"" << tmp << "\\n\" );" << endl;
			}
		} else {
			if ( idx_stop != -1 ) {
				if ( isCode) {
					tmp = str.substr(0, idx_stop);
					trim(tmp, " \t\n\r");

					if ( tmp.size() > 0 ) file << "\t" << tmp << endl;

					tmp = str.substr(idx_stop+2);
					tmp = correctString(tmp);
					if ( tmp.size() > 0 ) file << "\tprintf ( \"" << tmp << "\\n\" );" << endl;

					isCode = false;
				} else {
					cerr << "ERROR: Invalid tag in line " << count << "." << endl;
					exit(1);
				}

			} else {
				if(isCode) {
					file << "\t" << str << endl;
				} else {
					tmp = correctString(str);
					if ( tmp.size() > 0 ) file << "\tprintf ( \"" << tmp << "\\n\" );" << endl;
				}
			}
		}


	}

	file_tmp.close();

	/* footer */
	file << "}" << endl;

	file.close();
}

void cleanView(const string module) {
	string filename = getViewFilename(module);
	filename += ".tmp";
    remove(filename.c_str());                   /* remove temp-file */
}

string directiveInclude(string parameter) {
	int idx_start = parameter.find("file=\"");
	if (idx_start == -1) {
		cerr << "ERROR:\t\tInvalid parameter (no valid file-attribute)." << endl;
		exit(1);
	}

	int idx_end = parameter.find("\"", idx_start+6);
	if (idx_end == -1) {
		cerr << "ERROR:\t\tInvalid parameter (attribute not closed)." << endl;
		exit(1);
	}

	string filename = CONFIG_WEB_DIRECTORY;
	filename += "/";
	filename += parameter.substr(idx_start+6, idx_end-idx_start-6);

	cout << "DIRECTIVE:\tInclude (" << filename << ")" << endl;

	string line;
	string content;
	fstream file;
	file.open(filename.c_str(), ios::in);
	if (!file.is_open()) {
		cerr << "ERROR:\t\tUnable to read include-file (" << filename << ")." << endl;
		exit(1);
	}

	while (file.good()) {
		getline(file, line);
		content += line+ "\n";
	}

	file.close();

	trim(content, " \t\n\r");
	return content;
}
