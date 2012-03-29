
#include "database.h"
#include "database-postgresql.h"

#include "log.h"
#include "common.h"

#include <iostream>
#include <cstdlib>

string escapeParameter(string query) {
	char *out = (char *)malloc(query.size()*2+1);
	PQescapeString(out, query.c_str(), query.size());

	string result = out;
	
	free(out);
	return result;
}

int countParameter(string query) {
	int count = 0;
	int size = query.size();
	for(int i = 0; i < size; i++) {
		if (query[i] == '?') count++;

	}

	return count;
}

Database::Database() {
	init();
	this->config = NULL;
}

Database::Database(Config *config = NULL) {
	init();
	setConfig(config);
	open();
}

Database::~Database() {
	close();
}

void Database::init() {
	if (isconnection) close();
	isconnection = false;
	isresult = false;
	resultRow = -1;
	resultRowMax = -1;
	resultColMax = -1;
}

void Database::setConfig(Config *config) {
	this->config = config;
	this->log = config->getLog();
}

void Database::open() {
	string info;
	info += "dbname=" + config->get("database.name");
	info += " ";
	info += "host=" + config->get("database.host");
	info += " ";
	info += "user=" + config->get("database.user");
	info += " ";
	info += "password=" + config->get("database.password");
//	const char *info = "host=localhost dbname=symano user=symano";
	log->debug(info);

    if ( isconnection ) close();                /* check connection and close it if is open already */
	connection = PQconnectdb(info.c_str());

	isconnection = true;
}

void Database::close() {
	/* close result */
	if (isresult) {
		PQclear(result);
		isresult = false;
	}

	/* close connection */
	if ( isconnection ) {
		PQfinish(connection);
		isconnection = false;
	}
}

int Database::getStatus() {
	return PQstatus(connection);
}

bool Database::isError() {
	return (!getStatus());
}

string Database::getErrorMessage() {
	string msg = PQerrorMessage(this->connection);

	return msg;
}

void Database::execute(string query) {
	string msg = "(Database::execute) ";

    closeResult();                              /* close result if it has been opened by an other execution */

	result = PQexec(this->connection, query.c_str());
	isresult = true;

	if (!result) {
		msg += "Unable to execute \"" + query + "\".";
		log->error(msg);
	} else {
		switch(PQresultStatus(result)) {
			case PGRES_COMMAND_OK:
				msg += "Command successfull \"" + query + "\".";
				log->debug(msg);
				break;
			case PGRES_TUPLES_OK:
				resultRowMax = PQntuples(result);
				resultColMax = PQnfields(result);
				resultRow = 0;
				msg += "Request successfull \"" + query + "\".";
				msg += " => ";
				msg += toString(resultColMax);
				msg += "x";
				msg += toString(resultRowMax);
				log->debug(msg);
				break;
			default:
				msg += "Unregistered status \"" + query + "\".";
				msg += " => ";
				msg += PQresStatus(PQresultStatus(result));
				msg += ": ";
				msg += PQresultErrorMessage(result);
				log->error(msg);
		}
	}
}

void Database::execute(string query, string parameter[]) {
	string str  = query;
	int size = countParameter(query);

	for (int i=0; i<size; i++) {
		str = replaceFirst(str, "?", escapeParameter(parameter[i]));
	}

	execute(str);
}

int Database::getNextId() {
	if (!isconnection) log->error("No database-connection (getNextId).");

	PGresult *result = PQexec(this->connection, SQL_NEXTID);
	if (!result) {
		log->error("No result (getNextId).");
	}

	int id = atoi(PQgetvalue(result, 0,0));

	PQclear(result);

	return id;
}

int Database::getRandom() {
	if (!isconnection) log->error("No database-connection (getRandom).");

	PGresult *result = PQexec(this->connection, SQL_RANDOM);
	if (!result) {
		log->error("No result (getRandom).");
	}

	int rnd = atoi(PQgetvalue(result, 0,0));

	PQclear(result);

	return rnd;
}

bool Database::isConnection() {
	return this->isconnection;
}

bool Database::isResult() {
	return this->isresult;
}

int Database::getResultRow() {
	return this->resultRow;
}

int Database::getResultCount() {
	return this->resultRowMax;
}

bool Database::closeResult() {
	if (isresult) {
		PQclear(result);
		result = NULL;
		resultColMax = -1;
		resultRow = -1;
		resultRowMax = -1;
		isresult = false;

		return true;
	} else return false;
}

bool Database::nextResult() {
	if ( isresult && resultRow < resultRowMax ) {
		resultRow++;

		return true;
	} else {
		return false;
	}
}

bool Database::previousResult() {
	if ( isresult && resultRow > 0 ) {
		resultRow--;

		return true;
	} else {
		return false;
	}
}

bool Database::firstResult() {
	if (isresult) {
		resultRow = 0;

		return true;
	} else return false;
}

bool Database::lastResult() {
	if (isresult) {
		resultRow = resultRowMax - 1;

		return true;
	} else return false;
}

string Database::getString(int col) {
	if (!isresult || PQbinaryTuples(result)) return string();

	string res = PQgetvalue(result, resultRow, col);

	return res;
}

int Database::getInteger(int col) {
	string res = getString(col);
	if(res.empty()) return -1;

	return toInteger(res);
}
