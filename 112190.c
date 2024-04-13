BackgroundWorkerInitializeConnection(const char *dbname, const char *username, uint32 flags)
{
	BackgroundWorker *worker = MyBgworkerEntry;

	/* XXX is this the right errcode? */
	if (!(worker->bgw_flags & BGWORKER_BACKEND_DATABASE_CONNECTION))
		ereport(FATAL,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("database connection requirement not indicated during registration")));

	InitPostgres(dbname, InvalidOid, username, InvalidOid, NULL, (flags & BGWORKER_BYPASS_ALLOWCONN) != 0);

	/* it had better not gotten out of "init" mode yet */
	if (!IsInitProcessingMode())
		ereport(ERROR,
				(errmsg("invalid processing mode in background worker")));
	SetProcessingMode(NormalProcessing);
}