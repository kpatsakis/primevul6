static NTSTATUS ldapsrv_queue_reply_forced(struct ldapsrv_call *call,
					   struct ldapsrv_reply *reply)
{
	NTSTATUS status = ldapsrv_encode(call, reply);

	if (NT_STATUS_IS_OK(status)) {
		DLIST_ADD_END(call->replies, reply);
	}
	return status;
}