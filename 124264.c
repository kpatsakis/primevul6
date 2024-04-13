struct ldapsrv_reply *ldapsrv_init_reply(struct ldapsrv_call *call, uint8_t type)
{
	struct ldapsrv_reply *reply;

	reply = talloc_zero(call, struct ldapsrv_reply);
	if (!reply) {
		return NULL;
	}
	reply->msg = talloc_zero(reply, struct ldap_message);
	if (reply->msg == NULL) {
		talloc_free(reply);
		return NULL;
	}

	reply->msg->messageid = call->request->messageid;
	reply->msg->type = type;
	reply->msg->controls = NULL;

	return reply;
}