bool tipc_sk_overlimit1(struct sock *sk, struct sk_buff *skb)
{
	atomic_t *dcnt = &tipc_sk(sk)->dupl_rcvcnt;
	unsigned int lim = rcvbuf_limit(sk, skb) + atomic_read(dcnt);
	unsigned int qsize = sk->sk_backlog.len + sk_rmem_alloc_get(sk);

	return (qsize > lim * 90 / 100);
}