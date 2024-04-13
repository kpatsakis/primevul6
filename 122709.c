static void nfs_umountall_req(void)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	if ((nfs_server_mount_port == -1) || (!fs_mounted))
		/* Nothing mounted, nothing to umount */
		return;

	p = &(data[0]);
	p = rpc_add_credentials(p);

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req(PROG_MOUNT, MOUNT_UMOUNTALL, data, len);
}