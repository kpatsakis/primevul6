BackendRun(Port *port)
{
	/*
	 * Make sure we aren't in PostmasterContext anymore.  (We can't delete it
	 * just yet, though, because InitPostgres will need the HBA data.)
	 */
	MemoryContextSwitchTo(TopMemoryContext);

	PostgresMain(port->database_name, port->user_name);
}