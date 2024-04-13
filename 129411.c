void ip6_sk_redirect(struct sk_buff *skb, struct sock *sk)
{
	ip6_redirect(skb, sock_net(sk), sk->sk_bound_dev_if, sk->sk_mark);
}