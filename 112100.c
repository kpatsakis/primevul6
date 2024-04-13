socket_is_send_pending(void)
{
	return (PqSendStart < PqSendPointer);
}