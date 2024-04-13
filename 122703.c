static int nfs_read_reply(uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;
	int rlen;
	uchar *data_ptr;

	debug("%s\n", __func__);

	memcpy(&rpc_pkt.u.data[0], pkt, sizeof(rpc_pkt.u.reply));

	if (ntohl(rpc_pkt.u.reply.id) > rpc_id)
		return -NFS_RPC_ERR;
	else if (ntohl(rpc_pkt.u.reply.id) < rpc_id)
		return -NFS_RPC_DROP;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0]) {
		if (rpc_pkt.u.reply.rstatus)
			return -9999;
		if (rpc_pkt.u.reply.astatus)
			return -9999;
		return -ntohl(rpc_pkt.u.reply.data[0]);
	}

	if ((nfs_offset != 0) && !((nfs_offset) %
			(NFS_READ_SIZE / 2 * 10 * HASHES_PER_LINE)))
		puts("\n\t ");
	if (!(nfs_offset % ((NFS_READ_SIZE / 2) * 10)))
		putc('#');

	if (supported_nfs_versions & NFSV2_FLAG) {
		rlen = ntohl(rpc_pkt.u.reply.data[18]);
		data_ptr = (uchar *)&(rpc_pkt.u.reply.data[19]);
	} else {  /* NFSV3_FLAG */
		int nfsv3_data_offset =
			nfs3_get_attributes_offset(rpc_pkt.u.reply.data);

		/* count value */
		rlen = ntohl(rpc_pkt.u.reply.data[1 + nfsv3_data_offset]);
		/* Skip unused values :
			EOF:		32 bits value,
			data_size:	32 bits value,
		*/
		data_ptr = (uchar *)
			&(rpc_pkt.u.reply.data[4 + nfsv3_data_offset]);
	}

	if (((uchar *)&(rpc_pkt.u.reply.data[0]) - (uchar *)(&rpc_pkt) + rlen) > len)
			return -9999;

	if (store_block(data_ptr, nfs_offset, rlen))
			return -9999;

	return rlen;
}