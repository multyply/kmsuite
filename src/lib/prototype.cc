#include "prototype.h"

Prototype::Prototype() {
    config = new Config();                      /* init config */
    log = new Log(config);                      /* init logging */
#ifdef CONFIG_DATABASE
    database = new Database(config);            /* init database */
    log->setDatabase(database);                 /* log into the database */
    session = new Session(config, database);    /* init session */
#endif
    response = new Response(config);            /* init resonse-object */
    request = new Request(config);              /* init request-object */
}

Prototype::~Prototype() {
	delete response;
	delete request;
#ifdef CONFIG_DATABASE
	delete session;
	delete database;
#endif
	delete log;
	delete config;
}
