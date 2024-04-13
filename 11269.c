static void tsk_set_nagle(struct tipc_sock *tsk)
{
	struct sock *sk = &tsk->sk;

	tsk->maxnagle = 0;
	if (sk->sk_type != SOCK_STREAM)
		return;
	if (tsk->nodelay)
		return;
	if (!(tsk->peer_caps & TIPC_NAGLE))
		return;
	/* Limit node local buffer size to avoid receive queue overflow */
	if (tsk->max_pkt == MAX_MSG_SIZE)
		tsk->maxnagle = 1500;
	else
		tsk->maxnagle = tsk->max_pkt;
}