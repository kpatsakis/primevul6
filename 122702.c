static int nfs_umountall_reply(uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

	debug("%s\n", __func__);

	memcpy(&rpc_pkt.u.data[0], pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) > rpc_id)
		return -NFS_RPC_ERR;
	else if (ntohl(rpc_pkt.u.reply.id) < rpc_id)
		return -NFS_RPC_DROP;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus)
		return -1;

	fs_mounted = 0;
	memset(dirfh, 0, sizeof(dirfh));

	return 0;
}