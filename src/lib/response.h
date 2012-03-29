#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

#include "config.h"
#include "parameter.h"

using namespace std;

class Response {
	private:
		Config *config;
		Parameter *parameter;
		Log *log;
		void init();
	public:
		Response();
		Response(Config *config);
		~Response();
		void setConfig(Config *config);
		void addParameter(string key, string value);
		string getParameter(string key);
		void printHeader();
};

#endif
