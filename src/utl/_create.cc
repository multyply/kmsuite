/*
 * Symano Project
 *
 * Create - checks and creates the directory structure and files
 */
#include "utl.h"
#include "common.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

string getProjectName(const string project);
string getModuleName(const string module);
bool isFile(const string filename);
void copyFile(const string from, const string to);
void createDirectory(const string directory);
void createFile(const string file, const string content);
void createStructure(const string project);
void copyBinary(const string project);
void copyHeader(const string project);
void copyLibrary(const string project);
void createLibrary(const string project);
void createModule(const string project, const string module);
void createMake(const string project, const string module);

int main(int argc, char *argv[]) {

	copyright();

	if ( argc < 2 ) {
		cout	<< "ERROR: Invalid arguments." << endl;
		usage(string(argv[0]));
		exit(1);
	}

	string project = argv[1];
	cout << "PROJECT: " << project << endl;
	createStructure(project);


	/* copy project-files */
	copyBinary(project);
	copyHeader(project);
	copyLibrary(project);

	createLibrary(project);                     /* PROJECT.lib.cc & PROJECT.lib.h */

	if ( argc == 2 ) {
		createModule(project, "index");
		createMake(project, "index");
	} else {
		for ( int i = 2; i < argc; i++ ) {
			createModule(project, string(argv[i]));
			createMake(project, string(argv[i]));
		}
	}


	return(EXIT_SUCCESS);
}

void usage(const string program) {
	cout	<< program << " PROJECTNAME [MODULE1] [MODULE2] [MODULEn]" << endl;

	cout	<< "\tIf there is no \"module\" given, a new project with the default-module \"index\" will be created." << endl;
}

string getProjectName(const string project) {
	if (project.size()<1) {
		cerr << "ERROR: Project-name is emtpy" << endl;
		exit(1);
	}

	string tmp = project;
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

bool isFile(const string filename) {
	struct stat st;
	if ( stat(filename.c_str(), &st) == 0 ) {
		cout << "\tWARNING: File already exists (" << filename << ")." << endl;
		return true;
	} else return false;
}

void createDirectory(const string directory) {
	cout << "DIRECTORY: " << directory << endl;

	/* check if the directory already exists */
	struct stat st;
	if ( stat(directory.c_str(), &st) == 0 ) {
		cout << "\tWARNING: Directory already exists." << endl;
		return;
	}

	/* create directory */
	if ( mkdir(directory.c_str(), 0777) ) {
		cerr << "\tERROR: Unable to create directory." << endl;
		exit(1);
	}
}

void copyFile(const string from, const string to) {
	ifstream in(from.c_str(), ios::binary);
	ofstream out(to.c_str(), ios::binary);

	out << in.rdbuf();

	out.close();
	in.close();
}

void createFile(const string file, const string content) {
	cout << "FILE: " << file << endl;

	/* check if the file already exists */
	if (isFile(file)) return;

	/* create file */
	ofstream f;

	f.open(file.c_str());
	if ( !f.is_open() ) {
		cerr << "\tUnable to create file." << endl;
		exit(1);
	}

	f << content;
	f.close();
}

void createStructure(const string project) {
	createDirectory(project);
	createDirectory(project + "/bin");
	createDirectory(project + "/bin/lib");
	createDirectory(project + "/bin/utl");
	createDirectory(project + "/cfg");
	createDirectory(project + "/doc");
	createDirectory(project + "/log");
	createDirectory(project + "/src");
	createDirectory(project + "/src/lib");
	createDirectory(project + "/web");
	createDirectory(project + "/web/image");
	createDirectory(project + "/web/script");
	createDirectory(project + "/web/style");
}

void copyBinary(const string project) {
	string filename;

	/* create */
	filename = project + "/bin/utl/create";
	cout << "BINARY: " << filename << endl;
	copyFile(getHome() + "/bin/utl/create", filename);
	chmod(filename.c_str(), S_IXUSR|S_IRUSR); 

	/* compile */
	filename = project + "/bin/utl/compile";
	cout << "BINARY: " << filename << endl;
	copyFile(getHome() + "/bin/utl/compile", filename);
	chmod(filename.c_str(), S_IXUSR|S_IRUSR); 
}

void copyHeader(const string project) {
	string filename;

	/* common */
	string header[] = {"common", "config", "database-postgresql", "database", "log", "parameter", "prototype", "request", "response", "session"};

	for (unsigned int i=0; i<sizeof(header)/sizeof(header[0]); i++) {
		filename = project + "/src/lib/" + header[i] + ".h";
		cout << "HEADER: " << filename << endl;
		copyFile(getHome() + "/src/lib/" + header[i] + ".h", filename);
	}

}

void copyLibrary(const string project) {
	string filename;

	/* symano.a */
	filename = project + "/bin/lib/symano.a";
	cout << "LIBRARY: " << filename << endl;
	copyFile(getHome() + "/bin/lib/symano.a", filename);
}

void createLibrary(const string project) {
	string lcc;
	lcc += "#include \"" + project + ".lib.h\"\n";
	lcc += "\n";
	lcc += "using namespace std;\n";
	lcc += "\n";
	createFile(project + "/src/" + project + ".lib.cc", lcc);

	string lh;
	lh += "#ifndef " + toUpper(project) + "_H\n";
	lh += "#define " + toUpper(project) + "_H\n";
	lh += "\n";
	lh += "#include \"prototype.h\"\n";
	lh += "\n";
	lh += "using namespace std;\n";
	lh += "\n";
	lh += "class " + getProjectName(project) + " : public Prototype {\n";
	lh += "\tprivate:\n";
	lh += "\tpublic:\n";
	lh += "};\n";
	lh += "\n";
	lh += "#endif\n";
	lh += "\n";
	createFile(project + "/src/" + project + ".lib.h", lh);
}

void createModule(const string project, const string module) {
	/* View */
	string html;
	html += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n";
	html += "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
	html += "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
	html += "\t<head>\n";
	html += "\t\t<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\"/>\n";
	html += "\t\t<meta http-equiv=\"content-script-type\" content=\"text/javascript\"/>\n";
	html += "\t\t<meta name=\"robots\" content=\"index, follow\"/>\n";
	html += "\t\t<meta http-equiv=\"pragma\" content=\"no-cache\"/>\n";
	html += "\t\t<meta http-equiv=\"imagetoolbar\" content=\"no\"/>\n";
	html += "\t\t<meta name=\"generator\" content=\"Symano\"/>\n";
	html += "\t\t<meta name=\"author\" content=\"\"/>\n";
	html += "\t\t<meta name=\"company\" content=\"\"/>\n";
	html += "\t\t<meta name=\"description\" content=\"\"/>\n";
	html += "\t\t<meta name=\"keywords\" content=\"\"/>\n";
	html += "\t\t<script src=\"/script/default.js\" type=\"text/javascript\"></script>\n";
	html += "\t\t<link href=\"/style/default.css\" type=\"text/css\" rel=\"stylesheet\"/>\n";
	html += "\t\t<title><%=title%></title>\n";
	html += "\t</head>\n";
	html += "\t<body>\n";
	html += "\t\t<%\n";
	html += "\t\t\tprintf(\"<b>This is C++!</b>\");\n";
	html += "\t\t%>\n";
	html += "\t</body>\n";
	html += "<html>\n";
	createFile(project + "/web/" + module + ".html", html);

	/* Controller */
	string filename_cc = project;
	filename_cc += "/";
	filename_cc += SRC_DIRECTORY;
	filename_cc += "/";
	filename_cc += module;
	filename_cc += ".cc";
	cout << "FILE: " << filename_cc << endl;

	if (!isFile(filename_cc)) {
		fstream file_cc;
		file_cc.open(filename_cc.c_str(), fstream::out);
		if ( !file_cc ) {
			cerr << "ERROR: Unable to open module (" << filename_cc << ")." << endl;
			exit(1);
		}

		file_cc << "#include \"" << module << ".web.h\"" << endl << endl;
		file_cc << "void " << getModuleName(module) << "::service() {" << endl;
		file_cc << "\trequest->parseGet();" << endl << endl;
		file_cc << "\tresponse->addParameter(\"title\", \"" << getProjectName(project) << " - " << getModuleName(module) << "\");" << endl;
		file_cc << "}" << endl;

		file_cc.close();
	}

//	string cc;
//	cc += "#include \"" + module + ".web.h\"\n";
////	cc += "#include \"" + project + ".lib.h\"\n";
//	cc += "\n";
//	cc += "int main() {\n";
//	cc += "\t" + getModuleName(module) + " *" + module + "_obj = new " + getModuleName(module) + "();\n";
//	cc += "\n";
//	cc += "\t" + module + "_obj->request->parseGet();\n";
//	cc += "\t" + module + "_obj->response->addParameter(\"title\", \"This is the title!\");\n";
//	cc += "\t// Parameter *req_param = " + module + "_obj->request->getQuery();\n";
//	cc += "\t// " + module + "_obj->response->addParameter(\"user\", req_param->get(\"user\"));\n";
//	cc += "\n";
//	cc += "\t" + module + "_obj->response->printHeader();\n";
//	cc += "\t" + module + "_obj->printTemplate();\n";
//	cc += "\n";
//	cc += "\tdelete " + module + "_obj;\n";
//	cc += "}\n";
//	createFile(project + "/src/" + module + ".cc", cc);

	/* Header */
//	string h;
//	h += "#include \"prototype.h\"\n";
//	h += "\n";
//	h += "class " + getModuleName(module) + " : public Prototype {\n";
//	h += "\tpublic:\n";
//	h += "\tvoid printTemplate();\n";
//	h += "};\n";
//	createFile(project + "/src/" + module + ".web.h", h);
}

void createMake(const string project, const string module) {

	string file = project + "/Makefile";
	cout << "MAKEFILE: " << file << endl;

	/* check if the file already exists */
	bool exists;
	struct stat st;
	string home = getHome();
	exists = ( stat(file.c_str(), &st) == 0 );
	

	ofstream f;
	f.open(file.c_str(), ios::app);
	if ( !f.is_open() ) {
		cerr << "\tUnable to write file." << endl;
		exit(1);
	}

	if ( !exists ) {
		f << "#TYPE := CONFIG_TYPE=SIMPLE" << endl;
		f << "TYPE := CONFIG_TYPE=DATABASE" << endl;
		f << "CC := g++" << endl;
		f << "ARCH := 64";
		f << "#OPT = -ggdb -O0" << endl;
		f << "OPT = -O3" << endl;
		f << "INC := -I " << home << "/src/lib" << endl;
		f << "LIB :=" << endl;
		f << "ifeq ($(TYPE), DATABASE)" << endl;
		f << "INC += -I$(shell pg_config --includedir-server)" << endl;
		f << "INC += -I$(shell pg_config --includedir)" << endl;
		f << "LIB += -L$(shell pg_config --libdir)" << endl;
		f << "LIB += -lpq" << endl;
		f << "endif" << endl;
		f << "#LIB :=-ldl" << endl;
//		f << "#LIB += -L bin/lib -lsymano" << endl;
		f << "CFLAGS = $(OPT) -D $(TYPE) -m$(ARCH) -c -Wall $(INC) $(LIB)" << endl;
		f << "LDFLAGS = $(OPT) -D $(TYPE) -m$(ARCH) -O3 -Wall -std=c++0x $(INC) $(LIB)" << endl;
		f << "AR = ar" << endl;
		f << "ARFLAGS = rcv" << endl;
		f << endl;
		f << "all:" << endl;
//		f << "\t $(CC) $(LDFLAGS) -c -o bin/" << project << ".lib.o src/" << project << ".lib.cc " << endl;
		f << endl;
	}

	f << "#" << getModuleName(module) << endl;
	f << "\t" << home << "/bin/utl/compile . " << module << endl;
	f << "\t$(CC) $(LDFLAGS) -o bin/" << module << " src/" << module << ".cc src/" << module << ".web.cc " << "src/" << project << ".lib.cc " << home << "/bin/lib/chtml-$(ARCH).a" << endl;
	f << endl;

	f.close();
}

/*
void createIndex(string project) {
	string tmp;
	ofstream file;

	tmp = project + "/str/index.cc";
	file.open(tmp.c_str());
	file << "#include \"prototype.h\"" << endl;
	file << endl;
	file << "int main() {" << endl;
	file << "}" << endl;
	file << endl;
	file << "void printTitle() {\n\
		\tcout << \"INDEX\" << endl;\n\
		}" << endl;
	file.close();

	tmp = project;
	tmp += "/";
	tmp += TPL_DIRECTORY;
	tmp += "/";
	tmp += "index";
	tmp += ".";
	tmp += TPL_EXTENSION;

	file.open(tmp.c_str());
	file << "<html>" << endl;
	file << "\t<head>" << endl;
	file << "\t\t<title><%getTitle()%/></title>" << endl;
	file << "\t</head>" << endl;
	file << "\t<body>" << endl;
	file << "\t</body>" << endl;
	file << "</html>" << endl;
}
*/
