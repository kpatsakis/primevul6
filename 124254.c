NTSTATUS ldapsrv_queue_reply(struct ldapsrv_call *call, struct ldapsrv_reply *reply)
{
	NTSTATUS status = ldapsrv_encode(call, reply);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (call->reply_size > call->reply_size + reply->blob.length
	    || call->reply_size + reply->blob.length > LDAP_SERVER_MAX_REPLY_SIZE) {
		DBG_WARNING("Refusing to queue LDAP search response size "
			    "of more than %zu bytes\n",
			    LDAP_SERVER_MAX_REPLY_SIZE);
		TALLOC_FREE(reply->blob.data);
		return NT_STATUS_FILE_TOO_LARGE;
	}

	call->reply_size += reply->blob.length;

	DLIST_ADD_END(call->replies, reply);

	return status;
}