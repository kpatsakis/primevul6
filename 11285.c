static int tipc_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct net *net = sock_net(sock->sk);
	struct tipc_sioc_nodeid_req nr = {0};
	struct tipc_sioc_ln_req lnr;
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case SIOCGETLINKNAME:
		if (copy_from_user(&lnr, argp, sizeof(lnr)))
			return -EFAULT;
		if (!tipc_node_get_linkname(net,
					    lnr.bearer_id & 0xffff, lnr.peer,
					    lnr.linkname, TIPC_MAX_LINK_NAME)) {
			if (copy_to_user(argp, &lnr, sizeof(lnr)))
				return -EFAULT;
			return 0;
		}
		return -EADDRNOTAVAIL;
	case SIOCGETNODEID:
		if (copy_from_user(&nr, argp, sizeof(nr)))
			return -EFAULT;
		if (!tipc_node_get_id(net, nr.peer, nr.node_id))
			return -EADDRNOTAVAIL;
		if (copy_to_user(argp, &nr, sizeof(nr)))
			return -EFAULT;
		return 0;
	default:
		return -ENOIOCTLCMD;
	}
}