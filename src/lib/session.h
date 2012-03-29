#ifndef SESSION_H
#define SESSION_H

#define SESSION_ID_ID 6
#define SESSION_ID_IP 3
#define SESSION_PARAMETER_DELETE "*** SYMANO-DELETE ***"
#define SESSION_SQL_CREATE "INSERT INTO system.session (id, sid, ip) VALUES (?,'?',?)"
#define SESSION_SQL_LOAD "SELECT data FROM system.session WHERE id=? AND sid='?' LIMIT 1"
#define SESSION_SQL_PARAMETER_GET "SELECT value FROM system.parameter WHERE key_session=? AND key='?' LIMIT 1"
#define SESSION_SQL_PARAMETER_TRUNCATE "DELETE FROM system.parameter WHERE key_session=?"
#define SESSION_SQL_PARAMETER_DELETE "DELETE FROM system.parameter WHERE key_session=? AND key='?'"
#define SESSION_SQL_PARAMETER_INSERT "INSERT INTO system.parameter (key_session, key, value) VALUES (?, '?', '?') LIMIT 1"
#define SESSION_SQL_PARAMETER_UPDATE "UPDATE system.parameter SET value='?' WHERE key_session=? AND key='?'"


#include <string>

#include "config.h"
#include "database.h"
#include "parameter.h"
#include "log.h"

using namespace std;

class Session {
	private:
		Config *config;
		Database *database;
		Parameter *parameter;
		Log *log;
		int id[9];
/*
 * 0 - timestamp
 * 1 - random value based on the timestamp
 * 2 - browser-id
 * 3 - client-ip
 * 4 - browser-accept
 * 5 - random value based on the operatingsystem
 * 6 - database-session-id
 * 7 - random value based on the database
 * 8 - checksum
 */
		bool new_session;
		bool new_header;
		bool has_changed;
		string getCookie();
		string generateId();
		void init();
		void resetId();
		bool checkId();
		string idToString();
		bool stringToId(string strid);
		void createSession();
	public:
		Session();
		Session(Config *config);
		Session(Config *config, Database *database);
		~Session();
		void setConfig(Config *config);
		void setDatabase(Database *database);
		void addParameter(string key, string value);
		string getParameter(string key);
		void delParameter(string key);
		void delAllParameter();
		void load();
		void save();
		void printHeader();
		string getId();
};

#endif
