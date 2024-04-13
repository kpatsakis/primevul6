static NTSTATUS ldapsrv_AbandonRequest(struct ldapsrv_call *call)
{
	struct ldap_AbandonRequest *req = &call->request->r.AbandonRequest;
	struct ldapsrv_call *c = NULL;
	struct ldapsrv_call *n = NULL;

	DEBUG(10, ("AbandonRequest\n"));

	for (c = call->conn->pending_calls; c != NULL; c = n) {
		n = c->next;

		if (c->request->messageid != req->messageid) {
			continue;
		}

		DLIST_REMOVE(call->conn->pending_calls, c);
		TALLOC_FREE(c);
	}

	return NT_STATUS_OK;
}