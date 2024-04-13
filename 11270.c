static void tsk_rej_rx_queue(struct sock *sk, int error)
{
	struct sk_buff *skb;

	while ((skb = __skb_dequeue(&sk->sk_receive_queue)))
		tipc_sk_respond(sk, skb, error);
}