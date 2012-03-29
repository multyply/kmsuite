#include "common.h"
#include "config.h"
#include "log.h"
#ifdef CONFIG_DATABASE
	#include "database.h"
	#include "session.h"
#endif
#include "response.h"
#include "request.h"
#include "parameter.h"

class Prototype {
	public:
		Prototype();
		~Prototype();
		Config *config;
		Log *log;
#ifdef CONFIG_DATABASE
		Database *database;
		Session *session;
#endif
		Response *response;
		Request *request;
};
