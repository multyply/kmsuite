
#ifndef DATABASE_H
#define DATABASE_H

#define DATABASE_TYPE "PROTOTYPE"

#include <string>
#ifdef CONFIG_DATABASE
	#if CONFIG_DATABASE == postgresql
		#include <libpq-fe.h>
	#endif
#endif
#include "config.h"

using namespace std;

class Database {
	private:
		Config *config;
		bool isconnection;
		bool isresult;
#ifdef CONFIG_DATABASE
	#if CONFIG_DATABASE == postgresql
		PGconn *connection;
		PGresult *result;
	#endif
#endif
		int resultRow;
		int resultRowMax;
		int resultColMax;
		Log *log;
		void init();
	public:
		Database();
		Database(Config *config);
		~Database();
		void setConfig(Config *config);
		void open();
		void close();
		int getStatus();
		bool isError();
		bool isConnection();
		string getErrorMessage();
		void execute(string query);
		void execute(string query, string parameter[]);
		int getNextId();
		int getRandom();
		bool isResult();
		int getResultRow();
		int getResultCount();
		bool closeResult();
		bool nextResult();
		bool previousResult();
		bool firstResult();
		bool lastResult();
		string getString(int col);
		int getInteger(int col);
};

#endif
