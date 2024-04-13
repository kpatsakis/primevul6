static void nfs_handler(uchar *pkt, unsigned dest, struct in_addr sip,
			unsigned src, unsigned len)
{
	int rlen;
	int reply;

	debug("%s\n", __func__);

	if (len > sizeof(struct rpc_t))
		return;

	if (dest != nfs_our_port)
		return;

	switch (nfs_state) {
	case STATE_PRCLOOKUP_PROG_MOUNT_REQ:
		if (rpc_lookup_reply(PROG_MOUNT, pkt, len) == -NFS_RPC_DROP)
			break;
		nfs_state = STATE_PRCLOOKUP_PROG_NFS_REQ;
		nfs_send();
		break;

	case STATE_PRCLOOKUP_PROG_NFS_REQ:
		if (rpc_lookup_reply(PROG_NFS, pkt, len) == -NFS_RPC_DROP)
			break;
		nfs_state = STATE_MOUNT_REQ;
		nfs_send();
		break;

	case STATE_MOUNT_REQ:
		reply = nfs_mount_reply(pkt, len);
		if (reply == -NFS_RPC_DROP) {
			break;
		} else if (reply == -NFS_RPC_ERR) {
			puts("*** ERROR: Cannot mount\n");
			/* just to be sure... */
			nfs_state = STATE_UMOUNT_REQ;
			nfs_send();
		} else {
			nfs_state = STATE_LOOKUP_REQ;
			nfs_send();
		}
		break;

	case STATE_UMOUNT_REQ:
		reply = nfs_umountall_reply(pkt, len);
		if (reply == -NFS_RPC_DROP) {
			break;
		} else if (reply == -NFS_RPC_ERR) {
			debug("*** ERROR: Cannot umount\n");
			net_set_state(NETLOOP_FAIL);
		} else {
			puts("\ndone\n");
			net_set_state(nfs_download_state);
		}
		break;

	case STATE_LOOKUP_REQ:
		reply = nfs_lookup_reply(pkt, len);
		if (reply == -NFS_RPC_DROP) {
			break;
		} else if (reply == -NFS_RPC_ERR) {
			puts("*** ERROR: File lookup fail\n");
			nfs_state = STATE_UMOUNT_REQ;
			nfs_send();
		} else if (reply == -NFS_RPC_PROG_MISMATCH &&
			   supported_nfs_versions != 0) {
			/* umount */
			nfs_state = STATE_UMOUNT_REQ;
			nfs_send();
			/* And retry with another supported version */
			nfs_state = STATE_PRCLOOKUP_PROG_MOUNT_REQ;
			nfs_send();
		} else {
			nfs_state = STATE_READ_REQ;
			nfs_offset = 0;
			nfs_len = NFS_READ_SIZE;
			nfs_send();
		}
		break;

	case STATE_READLINK_REQ:
		reply = nfs_readlink_reply(pkt, len);
		if (reply == -NFS_RPC_DROP) {
			break;
		} else if (reply == -NFS_RPC_ERR) {
			puts("*** ERROR: Symlink fail\n");
			nfs_state = STATE_UMOUNT_REQ;
			nfs_send();
		} else {
			debug("Symlink --> %s\n", nfs_path);
			nfs_filename = basename(nfs_path);
			nfs_path     = dirname(nfs_path);

			nfs_state = STATE_MOUNT_REQ;
			nfs_send();
		}
		break;

	case STATE_READ_REQ:
		rlen = nfs_read_reply(pkt, len);
		if (rlen == -NFS_RPC_DROP)
			break;
		net_set_timeout_handler(nfs_timeout, nfs_timeout_handler);
		if (rlen > 0) {
			nfs_offset += rlen;
			nfs_send();
		} else if ((rlen == -NFSERR_ISDIR) || (rlen == -NFSERR_INVAL)) {
			/* symbolic link */
			nfs_state = STATE_READLINK_REQ;
			nfs_send();
		} else {
			if (!rlen)
				nfs_download_state = NETLOOP_SUCCESS;
			if (rlen < 0)
				debug("NFS READ error (%d)\n", rlen);
			nfs_state = STATE_UMOUNT_REQ;
			nfs_send();
		}
		break;
	}
}