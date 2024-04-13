static bool tsk_unreturnable(struct tipc_sock *tsk)
{
	return msg_dest_droppable(&tsk->phdr) != 0;
}