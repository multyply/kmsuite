#include <cstdlib>
#include <iostream>

#include "session.h"
#include "common.h"
#include "database.h"

Session::Session() {
	this->config = NULL;
}

Session::Session(Config *config) {
	init();
	this->database = NULL;
	setConfig(config);
}

Session::Session(Config *config, Database *database) {
	init();
	setConfig(config);
	setDatabase(database);
	load();
	printHeader();
}

Session::~Session() {
	save();
	delete this->parameter;
}

void Session::init() {
	this->new_session = false;
	this->new_header = true;
	this->has_changed = false;
	this->parameter = new Parameter();
	resetId();
}

void Session::setConfig(Config *config) {
	this->config = config;
	this->log = config->getLog();
}

void Session::setDatabase(Database *database) {
	this->database = database;
}

void Session::addParameter(string key, string value) {
	this->has_changed = true;
	parameter->add(key, value);
}

string Session::getParameter(string key) {
	string value = parameter->get(key);

	if ( value == SESSION_PARAMETER_DELETE ) return string(); /* deleted */

	if ( value.empty() ) {
		string dbparam[2];
		dbparam[0] = toString(this->id[6]);
		dbparam[1] = key;
		database->execute(SESSION_SQL_PARAMETER_GET, dbparam);
		if (database->getResultCount() == 0 ) return string();

		value = database->getString(0);         /* get value from database */

        addParameter(key, value);               /* cache the parameter */
	}

	return value;
}

void Session::delParameter(string key) {
	this->has_changed = true;
	parameter->del(key);
    parameter->add(key, SESSION_PARAMETER_DELETE);              /* mark as deleted */
}

void Session::delAllParameter() {
	this->has_changed = true;
	parameter->delAll();

	string dbparam[1];
	dbparam[0] = toString(this->id[6]);
	database->execute(SESSION_SQL_PARAMETER_TRUNCATE, dbparam);
}

string Session::idToString() {
	string strid;
	
	strid += "|";
	for (int i=0; i<9; i++) {
		strid += toString(this->id[i]);
		strid += "|";
	}

	return strid;
}

void Session::resetId() {
	this->id[0] = 0;
}

bool Session::stringToId(string strid) {
    resetId();                                  /* reset id */

	/* check for separators */
	int len = strid.size();
	if ( strid[0] != '|' || strid[len-1] != '|' ) return false;

	/* build array with tokens */
	int idx_start;
	int idx_end = 0;
	string tmp;

	for (int i = 0; i<9; i++) {
		idx_start = idx_end;
		idx_end = strid.find("|", idx_start+1);

		/* check if the delimiter has been found */
        if ( idx_end == -1 ) {
			resetId();
			return false;
		}

		tmp = strid.substr(idx_start+1, idx_end-idx_start-1);
		/* check if the string is a digit */
        if (!isDigit(tmp)) {
			resetId();
			return false;
		}

		/* write token to id */
		this->id[i] = toInteger(tmp);
	}

	return true;
}

/*
 * return the session-id which will be gotten from the cookie or will be generated, if not exists
 */
string Session::getId() {
	/* check if the id has been gotten already */
	if ( this->id[0] != 0 ) return idToString();

	/* get session-id from cookie */
	string strid = getCookie();
	if ( strid.empty() ) createSession();
	else {
		stringToId(strid);
		if (!checkId()) createSession();
	}

	return idToString();
}

void Session::createSession() {
	new_session = true;                         /* send sid to the client */

	generateId();
	string strid = idToString();
	
	string sp[3];
	sp[0] = toString(id[SESSION_ID_ID]);
	sp[1] = strid;
	sp[2] = toString(id[SESSION_ID_IP]);
	database->execute(SESSION_SQL_CREATE, sp);
}

/*
 * get the session-id by the set cookie-variable
 */
string Session::getCookie() {
	string id;
	char *env_cookie = getenv("HTTP_COOKIE");
	if ( env_cookie != NULL ) {
		string cookie = env_cookie;
		int len = cookie.size();
		int max = toInteger(this->config->get("session.size.max"));
		if ( len > max ) return id;

		int idx_start = cookie.find("_session=");
		int idx_end;
		if (idx_start > -1 ) {
			idx_end = cookie.find(";", idx_start);
			if ( idx_end > idx_start )  {
				id = cookie.substr(idx_start + 9, idx_end);
			} else {
				id = cookie.substr(idx_start + 9);
			}
		}
	}
	return id;

}

/*
 * build a unique identifier with unique and random parameters
 */
string Session::generateId() {
	time_t ltime = time(NULL);

	/* int random */
	int seed = config->getInteger("security.random.seed");
    srand(ltime^seed);

	char *env_agent = getenv("HTTP_USER_AGENT");
	char *env_ip = getenv("REMOTE_ADDR");
	char *env_accept = getenv("HTTP_ACCEPT");

	if (env_agent == NULL || env_ip == NULL || env_accept == NULL) {
		log->error("Invalid CGI-Parameters (generateId).");
	}

	this->id[0] = (int) ltime;
	this->id[1] = rand() % 1000;
	this->id[2] = stringToCRC(env_agent);
	this->id[3] = stringToCRC(env_ip);          /* SESSION_ID_IP */
	this->id[4] = stringToCRC(env_accept);
	this->id[5] = getRandomInt();
	this->id[6] = database->getNextId();        /* SESSION_ID_ID */
	this->id[7] = database->getRandom();
	if (this->id[5] < 0) this->id[5] *= -1;

	/* calculate the checksum */
	this->id[8] = (this->id[0] % this->id[1]) + (this->id[2] % this->id[3]) + (this->id[4] % this->id[5]) + (this->id[6] % this->id[7]);

	return idToString();
}

/*
 * checks if the set id is valid
 */
bool Session::checkId() {
	if ( this->id[0] == 0 ) return false;

	/* calculate the checksum */
	int checksum = (this->id[0] % this->id[1]) + (this->id[2] % this->id[3]) + (this->id[4] % this->id[5]) + (this->id[6] % this->id[7]);
	if ( checksum != id[8] ) {
		resetId();
		return false;
	}

	/* check AGENT, IP and ACCEPT */
	char *env_agent = getenv("HTTP_USER_AGENT");
	char *env_ip = getenv("REMOTE_ADDR");
	char *env_accept = getenv("HTTP_ACCEPT");
	
	if (env_agent == NULL || env_ip == NULL || env_accept == NULL) return false;

	if (this->id[2] != stringToCRC(env_agent)) return false;
	if (this->id[3] != stringToCRC(env_ip)) return false;
	if (this->id[4] != stringToCRC(env_accept)) return false;


/* check if there is an active session in the database */
	string l_param[] = {toString(id[SESSION_ID_ID]), idToString()};

	database->execute(SESSION_SQL_LOAD, l_param);
	return (database->getResultCount() == 1);   /* return true if the db-session was found */
}

void Session::load() {
	getId();
}

void Session::save() {
    if (!this->has_changed) return;             /* only write into database if anything has been changed */

	/* delete all session parameters */
	string dbparam[3];
	dbparam[0] = toString(this->id[6]);

	/* add parameters */
	string key;
	string value;
	for (int i=0; i<this->parameter->size(); i++) {
		key = this->parameter->getKey(i);
		value = this->parameter->get(key);
		dbparam[1] = key;
		if (value == SESSION_PARAMETER_DELETE) {
			/* delete entry */
			database->execute(SESSION_SQL_PARAMETER_DELETE, dbparam);
			break;
		}

		/* check if the entry already exists */
		database->execute(SESSION_SQL_PARAMETER_GET, dbparam);
		if (database->getResultCount() == 0) {
			/* insert new parameter */
			dbparam[2] = value;
			database->execute(SESSION_SQL_PARAMETER_INSERT, dbparam);
		} else if (database->getString(0) != value) {
			/* update parameter */
			dbparam[0] = value;
			dbparam[1] = toString(this->id[6]);
			dbparam[2] = key;
			database->execute(SESSION_SQL_PARAMETER_UPDATE, dbparam);
		}
	}

}

void Session::printHeader() {
	if (!this->new_header) return;              /* check if the header has been set already */
	if (config->get("system.interface") != CONFIG_SYSTEM_INTERFACE_WEB) return;

	cout << "Set-Cookie: _session=" << idToString() << endl;
	new_header = false;
}
