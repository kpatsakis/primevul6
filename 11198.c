int tsk_set_importance(struct sock *sk, int imp)
{
	if (imp > TIPC_CRITICAL_IMPORTANCE)
		return -EINVAL;
	msg_set_importance(&tipc_sk(sk)->phdr, (u32)imp);
	return 0;
}