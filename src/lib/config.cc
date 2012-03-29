#include "config.h"
#include <cstdlib>

Config::Config() {
	parameter = new Parameter();

	/* system */
	char *env = getenv("GATEWAY_INTERFACE");
	if (env == NULL) parameter->add("system.interface", CONFIG_SYSTEM_INTERFACE_CLI);
	else parameter->add("system.interface", CONFIG_SYSTEM_INTERFACE_WEB);

	/* security */
	parameter->add("security.random.seed", CONFIG_SECURITY_RANDOM_SEED);

	/* config */
	parameter->add("config.directory", string(CONFIG_ROOT_DIRECTORY) + string(CONFIG_CONFIG_DIRECTORY));

	/* log */
	parameter->add("log.directory", string(CONFIG_ROOT_DIRECTORY) + string(CONFIG_LOG_DIRECTORY));

	/* session */
	parameter->add("session.size.max", CONFIG_SESSION_SIZE_MAX);

	/* database */
#ifdef CONFIG_DATABASE
	parameter->add("database.type", CONFIG_DATABASE_TYPE);
	parameter->add("database.name", CONFIG_DATABASE_NAME);
	parameter->add("database.host", CONFIG_DATABASE_HOST);
	parameter->add("database.port", CONFIG_DATABASE_PORT);
	parameter->add("database.user", CONFIG_DATABASE_USER);
	parameter->add("database.password", CONFIG_DATABASE_PASSWORD);
	parameter->add("database.schema", CONFIG_DATABASE_SCHEMA);
#endif

	this->log = NULL;
}

Config::~Config() {
	delete parameter;
}

string Config::get(string key) {
	return parameter->get(key);
}

int Config::getInteger(string key) {
	return parameter->getInteger(key);
}

void Config::setLog(Log *log) {
	this->log = log;
}

Log *Config::getLog() {
	return this->log;
}
