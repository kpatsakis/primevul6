MaxLivePostmasterChildren(void)
{
	return 2 * (MaxConnections + autovacuum_max_workers + 1 +
				max_wal_senders + max_worker_processes);
}