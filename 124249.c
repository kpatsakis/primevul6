static NTSTATUS ldapsrv_unwilling(struct ldapsrv_call *call, int error)
{
	struct ldapsrv_reply *reply;
	struct ldap_ExtendedResponse *r;

	DEBUG(10,("Unwilling type[%d] id[%d]\n", call->request->type, call->request->messageid));

	reply = ldapsrv_init_reply(call, LDAP_TAG_ExtendedResponse);
	if (!reply) {
		return NT_STATUS_NO_MEMORY;
	}

	r = &reply->msg->r.ExtendedResponse;
	r->response.resultcode = error;
	r->response.dn = NULL;
	r->response.errormessage = NULL;
	r->response.referral = NULL;
	r->oid = NULL;
	r->value = NULL;

	ldapsrv_queue_reply(call, reply);
	return NT_STATUS_OK;
}