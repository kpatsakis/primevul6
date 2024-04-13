static void rpc_req(int rpc_prog, int rpc_proc, uint32_t *data, int datalen)
{
	struct rpc_t rpc_pkt;
	unsigned long id;
	uint32_t *p;
	int pktlen;
	int sport;

	id = ++rpc_id;
	rpc_pkt.u.call.id = htonl(id);
	rpc_pkt.u.call.type = htonl(MSG_CALL);
	rpc_pkt.u.call.rpcvers = htonl(2);	/* use RPC version 2 */
	rpc_pkt.u.call.prog = htonl(rpc_prog);
	switch (rpc_prog) {
	case PROG_NFS:
		if (supported_nfs_versions & NFSV2_FLAG)
			rpc_pkt.u.call.vers = htonl(2);	/* NFS v2 */
		else /* NFSV3_FLAG */
			rpc_pkt.u.call.vers = htonl(3);	/* NFS v3 */
		break;
	case PROG_PORTMAP:
	case PROG_MOUNT:
	default:
		rpc_pkt.u.call.vers = htonl(2);	/* portmapper is version 2 */
	}
	rpc_pkt.u.call.proc = htonl(rpc_proc);
	p = (uint32_t *)&(rpc_pkt.u.call.data);

	if (datalen)
		memcpy((char *)p, (char *)data, datalen*sizeof(uint32_t));

	pktlen = (char *)p + datalen * sizeof(uint32_t) - (char *)&rpc_pkt;

	memcpy((char *)net_tx_packet + net_eth_hdr_size() + IP_UDP_HDR_SIZE,
	       &rpc_pkt.u.data[0], pktlen);

	if (rpc_prog == PROG_PORTMAP)
		sport = SUNRPC_PORT;
	else if (rpc_prog == PROG_MOUNT)
		sport = nfs_server_mount_port;
	else
		sport = nfs_server_port;

	net_send_udp_packet(net_server_ethaddr, nfs_server_ip, sport,
			    nfs_our_port, pktlen);
}