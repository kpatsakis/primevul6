static void tipc_sk_set_orig_addr(struct msghdr *m, struct sk_buff *skb)
{
	DECLARE_SOCKADDR(struct sockaddr_pair *, srcaddr, m->msg_name);
	struct tipc_msg *hdr = buf_msg(skb);

	if (!srcaddr)
		return;

	srcaddr->sock.family = AF_TIPC;
	srcaddr->sock.addrtype = TIPC_SOCKET_ADDR;
	srcaddr->sock.scope = 0;
	srcaddr->sock.addr.id.ref = msg_origport(hdr);
	srcaddr->sock.addr.id.node = msg_orignode(hdr);
	srcaddr->sock.addr.name.domain = 0;
	m->msg_namelen = sizeof(struct sockaddr_tipc);

	if (!msg_in_group(hdr))
		return;

	/* Group message users may also want to know sending member's id */
	srcaddr->member.family = AF_TIPC;
	srcaddr->member.addrtype = TIPC_SERVICE_ADDR;
	srcaddr->member.scope = 0;
	srcaddr->member.addr.name.name.type = msg_nametype(hdr);
	srcaddr->member.addr.name.name.instance = TIPC_SKB_CB(skb)->orig_member;
	srcaddr->member.addr.name.domain = 0;
	m->msg_namelen = sizeof(*srcaddr);
}