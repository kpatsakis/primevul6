static int rpc_lookup_reply(int prog, uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

	memcpy(&rpc_pkt.u.data[0], pkt, len);

	debug("%s\n", __func__);

	if (ntohl(rpc_pkt.u.reply.id) > rpc_id)
		return -NFS_RPC_ERR;
	else if (ntohl(rpc_pkt.u.reply.id) < rpc_id)
		return -NFS_RPC_DROP;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus)
		return -1;

	switch (prog) {
	case PROG_MOUNT:
		nfs_server_mount_port = ntohl(rpc_pkt.u.reply.data[0]);
		break;
	case PROG_NFS:
		nfs_server_port = ntohl(rpc_pkt.u.reply.data[0]);
		break;
	}

	return 0;
}