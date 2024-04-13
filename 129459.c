void ip6_sk_update_pmtu(struct sk_buff *skb, struct sock *sk, __be32 mtu)
{
	ip6_update_pmtu(skb, sock_net(sk), mtu,
			sk->sk_bound_dev_if, sk->sk_mark);
}