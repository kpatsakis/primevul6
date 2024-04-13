static int tipc_sk_anc_data_recv(struct msghdr *m, struct sk_buff *skb,
				 struct tipc_sock *tsk)
{
	struct tipc_msg *hdr;
	u32 data[3] = {0,};
	bool has_addr;
	int dlen, rc;

	if (likely(m->msg_controllen == 0))
		return 0;

	hdr = buf_msg(skb);
	dlen = msg_data_sz(hdr);

	/* Capture errored message object, if any */
	if (msg_errcode(hdr)) {
		if (skb_linearize(skb))
			return -ENOMEM;
		hdr = buf_msg(skb);
		data[0] = msg_errcode(hdr);
		data[1] = dlen;
		rc = put_cmsg(m, SOL_TIPC, TIPC_ERRINFO, 8, data);
		if (rc || !dlen)
			return rc;
		rc = put_cmsg(m, SOL_TIPC, TIPC_RETDATA, dlen, msg_data(hdr));
		if (rc)
			return rc;
	}

	/* Capture TIPC_SERVICE_ADDR/RANGE destination address, if any */
	switch (msg_type(hdr)) {
	case TIPC_NAMED_MSG:
		has_addr = true;
		data[0] = msg_nametype(hdr);
		data[1] = msg_namelower(hdr);
		data[2] = data[1];
		break;
	case TIPC_MCAST_MSG:
		has_addr = true;
		data[0] = msg_nametype(hdr);
		data[1] = msg_namelower(hdr);
		data[2] = msg_nameupper(hdr);
		break;
	case TIPC_CONN_MSG:
		has_addr = !!tsk->conn_addrtype;
		data[0] = msg_nametype(&tsk->phdr);
		data[1] = msg_nameinst(&tsk->phdr);
		data[2] = data[1];
		break;
	default:
		has_addr = false;
	}
	if (!has_addr)
		return 0;
	return put_cmsg(m, SOL_TIPC, TIPC_DESTNAME, 12, data);
}