/*
 * Symano Project
 *
 * Compile - compiles templates to C-code
 */
#include "utl.h"
#include "common.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

string correctString(const string str);
string correctProjectName(const string project);
string geProjectName(const string project);
string getModuleName(const string module);
string getFilenameCC(const string project, const string module);
string getFilenameH(const string project, const string module);
string getFilenameTPL(const string project, const string module);
void check(const string project, const string module);
void transform(const string project, const string module);
void header(const string project, const string module);
void example(const string project, const string module);

int main(int argc, char *argv[]) {
	
	if ( argc < 2 ) {
		usage(string(argv[0]));
		exit(1);
	}
	string project = argv[1];
	cout << "PROJECT: " << project << endl;

	
	string module;
	if ( argc < 3 ) {
/* TODO: list directory and transform all templates */
	} else {
		/* transform the given templates */
		
		for ( int i = 2; i<argc ;i++ ) {
			module = argv[i];
			check(project, module);
			transform(project, module);
			header(project, module);
//			example(project, module);
		}
	}

	return 0;
}


void usage(const string program) {
	cout << program << " PROJECTNAME [MODULENAME]" << endl;

	cout << "If there is no \"module\" given, all modules will be transformed." << endl;
}

string correctString(const string str) {
	string tmp = str;

	tmp = replace(tmp, "\\", "\\\\");
	tmp = replace(tmp, "\"", "\\\"");

//	trim(tmp, " \t\n\r");

	return tmp;
}

string correctProjectName(const string project) {
	if ( project == "." ) {
		char *path = get_current_dir_name();    /* get current directory */
		string tmp = path;
		free(path);                             /* free memory */
		int idx = tmp.find_last_of('/');
		if ( idx != -1 ) tmp = tmp.substr(++idx);
		return tmp;
	} else return project;
}

string getProjectName(const string project) {
	if (project.size()<1) {
		cerr << "ERROR: Project-name is emtpy" << endl;
		exit(1);
	}

	string tmp = correctProjectName(project);
	tmp[0] = toupper(tmp[0]);

	return tmp;
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

string getFilenameCC(const string project, const string module) {
	string filename = project;
	filename += "/";
	filename += SRC_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".web.cc";

	return filename;
}

string getFilenameH(const string project, const string module) {
	string filename = project;
	filename += "/";
	filename += SRC_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".web.h";

	return filename;
}

string getFilenameTPL(const string project, const string module) {
	string filename = project;
	filename += "/";
	filename += TPL_DIRECTORY;
	filename += "/";
	filename += module;
	filename += ".";
	filename += TPL_EXTENSION;

	return filename;
}

void check(const string project, const string module) {
	cout << "CHECK: " << getFilenameTPL(project, module) << endl;
	struct stat st;

	if ( stat(getFilenameTPL(project, module).c_str(), &st) != 0 ) {
		cerr << "ERROR: TemplateFile doesn't exists." << endl;
		exit(1);
	}
}
// FIX - only ONE C-Inclusion per line
void transform(const string project, const string module) {
	cout << "MODULE: " << module << endl;

	string filename_cc = getFilenameCC(project, module);

	fstream file_cc;
	file_cc.open(filename_cc.c_str(), fstream::out);
	if ( !file_cc ) {
		cerr << "ERROR: Unable to open module (" << filename_cc << ")." << endl;
		exit(1);
	}

	/* header */
	file_cc << "#include \"" << module << ".web.h\"" << endl << endl;
	file_cc << "#include <iostream>" << endl << endl;

	/* main */
	file_cc << "int main() {" << endl;
	file_cc << "\t" << getModuleName(module) << " *" << module << "_obj = new " << getModuleName(module) << "();" << endl << endl;
	file_cc << "\t" << module << "_obj->service();" << endl << endl;
	file_cc << "\t" << module << "_obj->response->printHeader();" << endl;
	file_cc << "\t" << module << "_obj->printTemplate();" << endl << endl;
	file_cc << "\tdelete " << module << "_obj;" << endl << endl;
	file_cc << "\treturn(0);" << endl;
	file_cc << "}" << endl << endl;
	
	/* printTemplate */
	file_cc << "void " << getModuleName(module) << "::printTemplate() {" << endl;
    file_cc << "\tprintf ( \"\\n\" );" << endl;

	string filename_tpl = getFilenameTPL(project, module);

	fstream file_tpl;
	file_tpl.open(filename_tpl.c_str(), fstream::in);
	if ( !file_tpl ) {
		cerr << "ERROR: Unable to open module (" << filename_tpl << ")." << endl;
		exit(1);
	}


//	string tdata((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	/* parse line by line */
	bool isCode = false;
	char line[TPL_LINE+1];
	int count = 0;
	int idx_start, idx_stop;
	string tmp;
	while(file_tpl.getline(line, TPL_LINE)) {
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
			if ( tmp.size() > 0 ) file_cc << "\tprintf ( \"" << tmp << "\" );" << endl;

			/* C-code */
			if ( idx_stop == -1 ) {
				isCode = true;
				tmp = str.substr(idx_start+2);
				tmp = correctString(tmp);

				/* template-code */
				if ( tmp.size() > 0 ) file_cc << "\t" << tmp << endl;
			} else {
				tmp = str.substr(idx_start+2, idx_stop-idx_start-2);
				trim(tmp, " \t\n\r");

				if ( tmp.size() > 0 ) {
					if ( tmp[0] == '=' ) {
						tmp = tmp.substr(1);
						trim(tmp, " ");
						file_cc << "\tprintf ( \"%s\", response->getParameter(\"" << tmp << "\").c_str() );" << endl;
					} else {
						file_cc << "\t" << tmp << endl;
					}
				}
				tmp = str.substr(idx_stop+2);
				tmp = correctString(tmp);
				if ( tmp.size() > 0 ) file_cc << "\tprintf ( \"" << tmp << "\\n\" );" << endl;
			}
		} else {
			if ( idx_stop != -1 ) {
				if ( isCode) {
					tmp = str.substr(0, idx_stop);
					trim(tmp, " \t\n\r");

					if ( tmp.size() > 0 ) file_cc << "\t" << tmp << endl;

					tmp = str.substr(idx_stop+2);
					tmp = correctString(tmp);
					if ( tmp.size() > 0 ) file_cc << "\tprintf ( \"" << tmp << "\\n\" );" << endl;

					isCode = false;
				} else {
					cerr << "ERROR: Invalid tag in line " << count << "." << endl;
					exit(1);
				}

			} else {
				if(isCode) {
					file_cc << "\t" << str << endl;
				} else {
					tmp = correctString(str);
					if ( tmp.size() > 0 ) file_cc << "\tprintf ( \"" << tmp << "\\n\" );" << endl;
				}
			}
		}


	}

	file_tpl.close();

	/* footer */
	file_cc << "}" << endl;

	file_cc.close();

}

void header(const string project, const string module) {
	string filename_h = getFilenameH(project, module);


	fstream file_h;
	file_h.open(filename_h.c_str(), fstream::out);
	if ( !file_h ) {
		cerr << "ERROR: Unable to open module (" << filename_h << ")." << endl;
		exit(1);
	}

	file_h << "#ifndef " << toUpper(module) << "_H" << endl;
	file_h << "#define " << toUpper(module) << "_H" << endl << endl;
	file_h << "#include \"" << correctProjectName(project) << ".lib.h\"" << endl << endl;
	file_h << "class " << getModuleName(module) << " : public " << getProjectName(project) << " {" << endl;
	file_h << "\tpublic:" << endl;
	file_h << "\t\tvoid service();" << endl;
	file_h << "\t\tvoid printTemplate();" << endl;
	file_h << "};" << endl << endl;
	file_h << "#endif" << endl;

	file_h.close();

}

void example(const string project, const string module) {
	string filename_cc = project;
	filename_cc += "/";
	filename_cc += SRC_DIRECTORY;
	filename_cc += "/";
	filename_cc += MOD_DIRECTORY;
	filename_cc += "/";
	filename_cc += module;
	filename_cc += ".cc";


	fstream file_cc;
	file_cc.open(filename_cc.c_str(), fstream::out);
	if ( !file_cc ) {
		cerr << "ERROR: Unable to open module (" << filename_cc << ")." << endl;
		exit(1);
	}

	file_cc << "#include \"" << module << ".web.h\"" << endl << endl;
	file_cc << "void " << getModuleName(module) << "::service() {" << endl;
	file_cc << "request->parseGet();" << endl << endl;
	file_cc << "response->addParameter(\"title\", \"" << getProjectName(project) << " - " << getModuleName(module) << "\");" << endl;
	file_cc << "}" << endl;

	file_cc.close();


}
