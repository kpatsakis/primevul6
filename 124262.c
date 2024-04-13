static NTSTATUS ldapsrv_AddRequest(struct ldapsrv_call *call)
{
	struct ldap_AddRequest *req = &call->request->r.AddRequest;
	struct ldap_Result *add_result;
	struct ldapsrv_reply *add_reply;
	TALLOC_CTX *local_ctx;
	struct ldb_context *samdb = call->conn->ldb;
	struct ldb_message *msg = NULL;
	struct ldb_dn *dn;
	const char *errstr = NULL;
	int result = LDAP_SUCCESS;
	int ldb_ret;
	unsigned int i,j;
	struct ldb_result *res = NULL;

	DEBUG(10, ("AddRequest"));
	DEBUGADD(10, (" dn: %s\n", req->dn));

	local_ctx = talloc_named(call, 0, "AddRequest local memory context");
	NT_STATUS_HAVE_NO_MEMORY(local_ctx);

	dn = ldb_dn_new(local_ctx, samdb, req->dn);
	NT_STATUS_HAVE_NO_MEMORY(dn);

	DEBUG(10, ("AddRequest: dn: [%s]\n", req->dn));

	msg = talloc(local_ctx, struct ldb_message);
	NT_STATUS_HAVE_NO_MEMORY(msg);

	msg->dn = dn;
	msg->num_elements = 0;
	msg->elements = NULL;

	if (req->num_attributes > 0) {
		msg->num_elements = req->num_attributes;
		msg->elements = talloc_array(msg, struct ldb_message_element, msg->num_elements);
		NT_STATUS_HAVE_NO_MEMORY(msg->elements);

		for (i=0; i < msg->num_elements; i++) {
			msg->elements[i].name = discard_const_p(char, req->attributes[i].name);
			msg->elements[i].flags = 0;
			msg->elements[i].num_values = 0;
			msg->elements[i].values = NULL;
			
			if (req->attributes[i].num_values > 0) {
				msg->elements[i].num_values = req->attributes[i].num_values;
				msg->elements[i].values = talloc_array(msg->elements, struct ldb_val,
								       msg->elements[i].num_values);
				NT_STATUS_HAVE_NO_MEMORY(msg->elements[i].values);

				for (j=0; j < msg->elements[i].num_values; j++) {
					msg->elements[i].values[j].length = req->attributes[i].values[j].length;
					msg->elements[i].values[j].data = req->attributes[i].values[j].data;			
				}
			}
		}
	}

	add_reply = ldapsrv_init_reply(call, LDAP_TAG_AddResponse);
	NT_STATUS_HAVE_NO_MEMORY(add_reply);

	if (result == LDAP_SUCCESS) {
		res = talloc_zero(local_ctx, struct ldb_result);
		NT_STATUS_HAVE_NO_MEMORY(res);
		ldb_ret = ldapsrv_add_with_controls(call, msg, call->request->controls, res);
		result = map_ldb_error(local_ctx, ldb_ret, ldb_errstring(samdb),
				       &errstr);
	}

	add_result = &add_reply->msg->r.AddResponse;
	add_result->dn = NULL;
	if ((res != NULL) && (res->refs != NULL)) {
		add_result->resultcode =  map_ldb_error(local_ctx,
							LDB_ERR_REFERRAL, NULL,
							&errstr);
		add_result->errormessage = (errstr?talloc_strdup(add_reply,errstr):NULL);
		add_result->referral = talloc_strdup(call, *res->refs);
	} else {
		add_result->resultcode = result;
		add_result->errormessage = (errstr?talloc_strdup(add_reply,errstr):NULL);
		add_result->referral = NULL;
	}
	talloc_free(local_ctx);

	return ldapsrv_queue_reply(call, add_reply);

}