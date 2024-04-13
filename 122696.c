static int nfs_readlink_reply(uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;
	int rlen;
	int nfsv3_data_offset = 0;

	debug("%s\n", __func__);

	memcpy((unsigned char *)&rpc_pkt, pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) > rpc_id)
		return -NFS_RPC_ERR;
	else if (ntohl(rpc_pkt.u.reply.id) < rpc_id)
		return -NFS_RPC_DROP;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0])
		return -1;

	if (!(supported_nfs_versions & NFSV2_FLAG)) { /* NFSV3_FLAG */
		nfsv3_data_offset =
			nfs3_get_attributes_offset(rpc_pkt.u.reply.data);
	}

	/* new path length */
	rlen = ntohl(rpc_pkt.u.reply.data[1 + nfsv3_data_offset]);

	if (((uchar *)&(rpc_pkt.u.reply.data[0]) - (uchar *)(&rpc_pkt) + rlen) > len)
		return -NFS_RPC_DROP;

	if (*((char *)&(rpc_pkt.u.reply.data[2 + nfsv3_data_offset])) != '/') {
		int pathlen;

		strcat(nfs_path, "/");
		pathlen = strlen(nfs_path);
		memcpy(nfs_path + pathlen,
		       (uchar *)&(rpc_pkt.u.reply.data[2 + nfsv3_data_offset]),
		       rlen);
		nfs_path[pathlen + rlen] = 0;
	} else {
		memcpy(nfs_path,
		       (uchar *)&(rpc_pkt.u.reply.data[2 + nfsv3_data_offset]),
		       rlen);
		nfs_path[rlen] = 0;
	}
	return 0;
}