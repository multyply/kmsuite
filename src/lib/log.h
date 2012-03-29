
#ifndef LOG_H
#define LOG_H

#include "config.h"
#include "database.h"


class Log {
	public:
		enum log_destination {
			log_console,
			log_filesystem,
			log_database
		};
		enum log_type {
			log_info,
			log_debug,
			log_error
		};

	private:
		string directory;
		Config *config;
		Database *database;
		enum log_destination destination;
		enum log_destination type;
		void init();
		void writeToConsole(const char *msg, log_type type);
		void writeToFilesystem(const char *msg, log_type type);
		void writeToDatabase(const char *msg, log_type type);
	public:
		Log();
		Log(Config *config);
		Log(Config *config, Database *database);
		~Log();
		void setConfig(Config *config);
		void setDatabase(Database *database);
		void setDestination(log_destination dest);
		string getTime();
		string getDate();
		void info(const char *msg);
		void debug(const char *msg);
		void error(const char *msg);
		void info(const char *msg, log_destination dest);
		void error(const char *msg, log_destination dest);
		void debug(const char *msg, log_destination dest);
		void info(string msg);
		void debug(string msg);
		void error(string msg);
		void info(string msg, log_destination dest);
		void error(string msg, log_destination dest);
		void debug(string msg, log_destination dest);
};

#endif
