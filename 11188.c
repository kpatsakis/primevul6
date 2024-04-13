static int tipc_sk_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	unsigned int before = sk_rmem_alloc_get(sk);
	struct sk_buff_head xmitq;
	unsigned int added;

	__skb_queue_head_init(&xmitq);

	tipc_sk_filter_rcv(sk, skb, &xmitq);
	added = sk_rmem_alloc_get(sk) - before;
	atomic_add(added, &tipc_sk(sk)->dupl_rcvcnt);

	/* Send pending response/rejected messages, if any */
	tipc_node_distr_xmit(sock_net(sk), &xmitq);
	return 0;
}