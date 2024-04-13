static void nfs_timeout_handler(void)
{
	if (++nfs_timeout_count > NFS_RETRY_COUNT) {
		puts("\nRetry count exceeded; starting again\n");
		net_start_again();
	} else {
		puts("T ");
		net_set_timeout_handler(nfs_timeout +
					NFS_TIMEOUT * nfs_timeout_count,
					nfs_timeout_handler);
		nfs_send();
	}
}