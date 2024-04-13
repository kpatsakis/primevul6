static void tsk_advance_rx_queue(struct sock *sk)
{
	trace_tipc_sk_advance_rx(sk, NULL, TIPC_DUMP_SK_RCVQ, " ");
	kfree_skb(__skb_dequeue(&sk->sk_receive_queue));
}