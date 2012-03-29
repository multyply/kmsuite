#include "config.h"
#include "response.h"
#include "log.h"

#include <iostream>
#include <fstream>
#include <string>


using namespace std;

Response::Response() {
	init();
	this->config = NULL;
}

Response::Response(Config *config = NULL) {
	init();
	setConfig(config);
}

void Response::init() {
	this->config = NULL;
	this->parameter = new Parameter();
}

Response::~Response() {
	delete this->parameter;
}

void Response::setConfig(Config *config) {
	this->config = config;
	this->log = config->getLog();
}

string Response::getParameter(string key) {
	if ( key.empty() ) {
		string tmp;
		return tmp;
	}

	return this->parameter->get(key);
}

void Response::addParameter(string key, string value) {
	this->parameter->add(key, value);
}

void Response::printHeader() {
	if (config->get("system.interface") == CONFIG_SYSTEM_INTERFACE_WEB) {
		cout << "Content-Type: text/html; charset=UTF-8" << endl << endl;
	}
}
