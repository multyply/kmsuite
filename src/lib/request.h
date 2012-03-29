#ifndef REQUEST_H
#define REQUEST_H

#include "parameter.h"
#include "config.h"
#include "log.h"

class Request {
	private:
		Config *config;
		Log *log;
		Parameter *query;
		void parseString(string query_str);
	public:
		Request();
		Request(Config *config);
		~Request();
		void init();
		void setConfig(Config *config);
		void parsePost();
		void parseGet();
		void parseQuery();
		Parameter *getQuery();
		string getParameter(string key);
		string getScriptName();
};

#endif
