#include "log.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

Log::Log() {
	init();
	this->config = NULL;
	this->database = NULL;
}

Log::Log(Config *config) {
	init();
	setConfig(config);
	this->database = NULL;
}

Log::Log(Config *config, Database *database) {
	init();
	setConfig(config);
	setDatabase(database);
}

Log::~Log() {

}

void Log::init() {
	setDestination(log_filesystem);
}

void Log::setConfig(Config *config) {
	this->config = config;
	this->directory = config->get("log.directory");
	this->config->setLog(this);
}

void Log::setDatabase(Database *database) {
	this->database = database;
}

void Log::setDestination(log_destination dest) {
	this->destination = dest;
}

/*
 * returns the current time as string
 */
string Log::getTime() {
	time_t ltime;
	struct tm *Tm;
	char buf[7];

	ltime = time(NULL);
	Tm = localtime(&ltime);
	
	sprintf(buf, "%02d:%02d:%02d", Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

	return string(buf);
}

/*
 * returns the current date as string
 */
string Log::getDate() {
	time_t ltime;
	struct tm *Tm;
	char buf[9];

	ltime = time(NULL);
	Tm = localtime(&ltime);
	
	sprintf(buf, "%02d.%02d.%4d", Tm->tm_mday, Tm->tm_mon+1, Tm->tm_year+1900);

	return string(buf);
}

void Log::writeToConsole(const char *msg, log_type type) {
	switch(type) {
		case log_info:
			printf ( "INFO" );
			break;
		case log_debug:
			printf ( "DEBUG" );
			break;
		case log_error:
			printf ( "ERROR" );
			break;
		default:
			printf ( "ERROR (writeToConsole): Invalid type\n" );
			exit(EXIT_FAILURE);
	}

	printf ( " (%s %s): %s\n", getDate().c_str(), getTime().c_str(), msg );

	/* exit if type is "error" */
	if ( type == log_error) exit(EXIT_FAILURE);
}

void Log::writeToFilesystem(const char *msg, log_type type) {
	string filename;
	filename = this->directory;
	filename += "/";
	filename += getDate();
	filename += ".log";


	FILE *file;
	file = fopen(filename.c_str(), "a");

	if ( file == NULL ) {
		string msg;
		msg += "ERROR (writeToFilesystem): Unable to open file \"";
		msg += filename;
		msg += "\".";
		writeToConsole(msg.c_str(), log_error);
	}

	switch(type) {
		case log_info:
			fprintf ( file, "INFO" );
			break;
		case log_debug:
			fprintf ( file, "DEBUG" );
			break;
		case log_error:
			fprintf ( file, "ERROR" );
			break;
		default:
			printf ( "ERROR (writeToFilesystem): Invalid type\n" );
			exit(EXIT_FAILURE);
	}

	fprintf ( file, " (%s): %s\n", getTime().c_str(), msg );

	fclose(file);

	/* exit if type is "error" */
	if ( type == log_error) exit(EXIT_FAILURE);

}

void Log::writeToDatabase(const char *msg, log_type type) {
	printf ( "Not implemented yet!\n" );
	exit(EXIT_FAILURE);
}

void Log::info(const char *msg) {
	info(msg, this->destination);
}

void Log::debug(const char *msg) {
	debug(msg, this->destination);
}

void Log::error(const char *msg) {
	error(msg, this->destination);
}

void Log::info(const char *msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg, log_info);
			break;
		case log_filesystem:
			writeToFilesystem(msg, log_info);
			break;
		case log_database:
			writeToDatabase(msg, log_info);
			break;
		default:
			printf ( "ERROR (log-info): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

void Log::debug(const char *msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg, log_debug);
			break;
		case log_filesystem:
			writeToFilesystem(msg, log_debug);
			break;
		case log_database:
			writeToDatabase(msg, log_debug);
			break;
		default:
			printf ( "ERROR (log-debug): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

void Log::error(const char *msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg, log_error);
			break;
		case log_filesystem:
			writeToFilesystem(msg, log_error);
			break;
		case log_database:
			writeToDatabase(msg, log_error);
			break;
		default:
			printf ( "ERROR (log-error): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

void Log::info(string msg) {
	info(msg.c_str(), this->destination);
}

void Log::debug(string msg) {
	debug(msg.c_str(), this->destination);
}

void Log::error(string msg) {
	error(msg.c_str(), this->destination);
}

void Log::info(string msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg.c_str(), log_info);
			break;
		case log_filesystem:
			writeToFilesystem(msg.c_str(), log_info);
			break;
		case log_database:
			writeToDatabase(msg.c_str(), log_info);
			break;
		default:
			printf ( "ERROR (log-info): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

void Log::debug(string msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg.c_str(), log_debug);
			break;
		case log_filesystem:
			writeToFilesystem(msg.c_str(), log_debug);
			break;
		case log_database:
			writeToDatabase(msg.c_str(), log_debug);
			break;
		default:
			printf ( "ERROR (log-debug): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

void Log::error(string msg, log_destination dest) {
	switch(dest) {
		case log_console:
			writeToConsole(msg.c_str(), log_error);
			break;
		case log_filesystem:
			writeToFilesystem(msg.c_str(), log_error);
			break;
		case log_database:
			writeToDatabase(msg.c_str(), log_error);
			break;
		default:
			printf ( "ERROR (log-error): Invalid destination\n" );
			exit(EXIT_FAILURE);
	}
}

