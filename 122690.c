void nfs_start(void)
{
	debug("%s\n", __func__);
	nfs_download_state = NETLOOP_FAIL;

	nfs_server_ip = net_server_ip;
	nfs_path = (char *)nfs_path_buff;

	if (nfs_path == NULL) {
		net_set_state(NETLOOP_FAIL);
		printf("*** ERROR: Fail allocate memory\n");
		return;
	}

	if (!net_parse_bootfile(&nfs_server_ip, nfs_path,
				sizeof(nfs_path_buff))) {
		sprintf(nfs_path, "/nfsroot/%02X%02X%02X%02X.img",
			net_ip.s_addr & 0xFF,
			(net_ip.s_addr >>  8) & 0xFF,
			(net_ip.s_addr >> 16) & 0xFF,
			(net_ip.s_addr >> 24) & 0xFF);

		printf("*** Warning: no boot file name; using '%s'\n",
		       nfs_path);
	}

	nfs_filename = basename(nfs_path);
	nfs_path     = dirname(nfs_path);

	printf("Using %s device\n", eth_get_name());

	printf("File transfer via NFS from server %pI4; our IP address is %pI4",
	       &nfs_server_ip, &net_ip);

	/* Check if we need to send across this subnet */
	if (net_gateway.s_addr && net_netmask.s_addr) {
		struct in_addr our_net;
		struct in_addr server_net;

		our_net.s_addr = net_ip.s_addr & net_netmask.s_addr;
		server_net.s_addr = nfs_server_ip.s_addr & net_netmask.s_addr;
		if (our_net.s_addr != server_net.s_addr)
			printf("; sending through gateway %pI4",
			       &net_gateway);
	}
	printf("\nFilename '%s/%s'.", nfs_path, nfs_filename);

	if (net_boot_file_expected_size_in_blocks) {
		printf(" Size is 0x%x Bytes = ",
		       net_boot_file_expected_size_in_blocks << 9);
		print_size(net_boot_file_expected_size_in_blocks << 9, "");
	}
	printf("\nLoad address: 0x%lx\nLoading: *\b", load_addr);

	net_set_timeout_handler(nfs_timeout, nfs_timeout_handler);
	net_set_udp_handler(nfs_handler);

	nfs_timeout_count = 0;
	nfs_state = STATE_PRCLOOKUP_PROG_MOUNT_REQ;

	/*nfs_our_port = 4096 + (get_ticks() % 3072);*/
	/*FIX ME !!!*/
	nfs_our_port = 1000;

	/* zero out server ether in case the server ip has changed */
	memset(net_server_ethaddr, 0, 6);

	nfs_send();
}