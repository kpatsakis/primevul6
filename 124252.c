static NTSTATUS ldapsrv_expired(struct ldapsrv_call *call)
{
	struct ldapsrv_reply *reply = NULL;
	struct ldap_ExtendedResponse *r = NULL;

	DBG_DEBUG("Sending connection expired message\n");

	reply = ldapsrv_init_reply(call, LDAP_TAG_ExtendedResponse);
	if (reply == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	/*
	 * According to RFC4511 section 4.4.1 this has a msgid of 0
	 */
	reply->msg->messageid = 0;

	r = &reply->msg->r.ExtendedResponse;
	r->response.resultcode = LDB_ERR_UNAVAILABLE;
	r->response.errormessage = "The server has timed out this connection";
	r->oid = "1.3.6.1.4.1.1466.20036"; /* see rfc4511 section 4.4.1 */

	ldapsrv_queue_reply(call, reply);
	return NT_STATUS_OK;
}