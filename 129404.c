static void __net_exit ipv6_inetpeer_exit(struct net *net)
{
	struct inet_peer_base *bp = net->ipv6.peers;

	net->ipv6.peers = NULL;
	inetpeer_invalidate_tree(bp);
	kfree(bp);
}