#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include "parameter.h"

/* system */
#define CONFIG_SYSTEM_INTERFACE_WEB "web"
#define CONFIG_SYSTEM_INTERFACE_CLI "cli"

/* security */
#define CONFIG_SECURITY_RANDOM_SEED "1024"      /* seed for pseudo-random-value */

/* parameter */
#define CONFIG_PARAMETER_KEY_SIZE_MAX 64        /* 64 Byte */
#define CONFIG_PARAMETER_VALUE_SIZE_MAX 102400  /* 100 KB */
#define CONFIG_PARAMETER_QUERY_SIZE_MAX 1024000 /* 1 MB */

/* database */
#ifdef CONFIG_DATABASE
	#define CONFIG_DATABASE_TYPE "postgresql"
	#define CONFIG_DATABASE_NAME "chtml"
	#define CONFIG_DATABASE_HOST "127.0.0.1"
	#define CONFIG_DATABASE_PORT "5432"
	#define CONFIG_DATABASE_USER "chtml"
	#define CONFIG_DATABASE_PASSWORD "chtml"
	#define CONFIG_DATABASE_SCHEMA "chtml"
#endif

/* root */
#define CONFIG_ROOT_DIRECTORY "../../"

/* source */
#define CONFIG_SOURCE_DIRECTORY "src"

/* binary */
#define CONFIG_BINARY_DIRECTORY "bin"

/* log */
#define CONFIG_CONFIG_DIRECTORY "cfg"

/* log */
#define CONFIG_LOG_DIRECTORY "log"

/* image */
#define CONFIG_WEB_DIRECTORY "web"

/* module */
#define CONFIG_MODULE_DIRECTORY "mod"

/* session */
#define CONFIG_SESSION_SIZE_MAX "1024"          /* 1 kB */

#include "parameter.h"

using namespace std;

class Log;

class Config {
	private:
		Parameter *parameter;
		Log *log;
	public:
		Config();
		~Config();
		string get(string key);
		int getInteger(string key);
		void setLog(Log *log);
		Log *getLog();
};

#endif

