#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "common.h"
#include "request.h"
#include "parameter.h"

using namespace std;

Request::Request() {
	init();
	this->config = NULL;
}

Request::Request(Config *config) {
	init();
	setConfig(config);
}

Request::~Request() {
	delete this->query;
}

void Request::init() {
//	char *env = getenv("GATEWAY_INTERFACE");

	/* environment-check */
//	if (env == NULL) {
//		cerr << "ERROR:\t\tNo gateway-interface found." << endl;
//		exit(1);
//	}

	this->query = new Parameter();
}

void Request::setConfig(Config *config) {
	this->config = config;
	this->log = config->getLog();
}

void Request::parseString(string query_str) {
	char *key;
	char *value;
	char *str = new char[query_str.size()+1];
	strcpy(str, query_str.c_str());

	char *token = strtok(str, "&");
	while(token) {
		key = token;
		while(*token && (*token != '=')) token++;
		if(*token) {
			*(token++) = '\0';
			value = token;
			if (value[0] != '\0') {
                string tmp = toURLDecode(value); /* decode URL-query */
				if (tmp.empty()) log->error("(Request::parseGet) Invalid query.");

                this->query->add(key, tmp);     /* add token to parameter */
			}
		}

		token = strtok((char *)0, "&");
	}

	delete[] str;
}

void Request::parseGet() {
	char *query = (char *)getenv("QUERY_STRING");
	if ( query == NULL || strlen(query) < 3) return;
	if (strlen(query) > CONFIG_PARAMETER_QUERY_SIZE_MAX) {
		log->info("Post is to big.");
		return;
	}

	parseString(query);
}

void Request::parsePost() {
	char *cl = getenv("CONTENT_LENGTH");
	if (cl == NULL) return;

	unsigned int len = toInteger(string(cl));
	if (len > CONFIG_PARAMETER_QUERY_SIZE_MAX) {
		log->info("Post is to big.");
		return;
	}

	string query;
	cin >> query;

	if (query.size() == 0) return;              /* no post */

	if (len != query.size()) {
		log->error("Invalid post-size.");
		return;
	}

	parseString(query);
}

void Request::parseQuery() {
	parseGet();
	parsePost();
}

Parameter *Request::getQuery() {
	return this->query;
}

string Request::getParameter(string key) {
	return this->query->get(key);
}

string Request::getScriptName() {
	char *env = getenv("SCRIPT_NAME");
	if (env == NULL) {
		cerr << "ERROR:\t\tNo script-name found." << endl;
		exit(1);
	}

	string result;
	result = env;

	return result;
}
