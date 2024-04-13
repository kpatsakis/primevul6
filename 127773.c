int isdn_ppp_unregister_compressor(struct isdn_ppp_compressor *ipc)
{
	if (ipc->prev)
		ipc->prev->next = ipc->next;
	else
		ipc_head = ipc->next;
	if (ipc->next)
		ipc->next->prev = ipc->prev;
	ipc->prev = ipc->next = NULL;
	return 0;
}