static int tipc_bind(struct socket *sock, struct sockaddr *skaddr, int alen)
{
	struct tipc_uaddr *ua = (struct tipc_uaddr *)skaddr;
	u32 atype = ua->addrtype;

	if (alen) {
		if (!tipc_uaddr_valid(ua, alen))
			return -EINVAL;
		if (atype == TIPC_SOCKET_ADDR)
			return -EAFNOSUPPORT;
		if (ua->sr.type < TIPC_RESERVED_TYPES) {
			pr_warn_once("Can't bind to reserved service type %u\n",
				     ua->sr.type);
			return -EACCES;
		}
	}
	return tipc_sk_bind(sock, skaddr, alen);
}