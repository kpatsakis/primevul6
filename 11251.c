bool tipc_sk_overlimit2(struct sock *sk, struct sk_buff *skb)
{
	unsigned int lim = rcvbuf_limit(sk, skb);
	unsigned int qsize = sk_rmem_alloc_get(sk);

	return (qsize > lim * 90 / 100);
}