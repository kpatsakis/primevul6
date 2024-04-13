static  bool tsk_unreliable(struct tipc_sock *tsk)
{
	return msg_src_droppable(&tsk->phdr) != 0;
}