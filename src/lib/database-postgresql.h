
#ifndef DATABASE_POSTGRESQL_H
#define DATABASE_POSTGRESQL_H

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#undef DATABASE_TYPE
#define DATABASE_TYPE "POSTGRESQL"

#define SQL_NEXTID "SELECT nextval('system.session_id_seq'::regclass)"
#define SQL_RANDOM "SELECT (random()*65535)::int"

#endif
