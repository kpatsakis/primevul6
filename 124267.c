static int ldap_server_search_callback(struct ldb_request *req, struct ldb_reply *ares)
{
	struct ldapsrv_context *ctx = talloc_get_type(req->context, struct ldapsrv_context);
	struct ldapsrv_call *call = ctx->call;
	struct ldb_context *ldb = call->conn->ldb;
	unsigned int j;
	struct ldapsrv_reply *ent_r = NULL;
	struct ldap_SearchResEntry *ent;
	int ret;
	NTSTATUS status;

	if (!ares) {
		return ldb_request_done(req, LDB_ERR_OPERATIONS_ERROR);
	}
	if (ares->error != LDB_SUCCESS) {
		return ldb_request_done(req, ares->error);
	}

	switch (ares->type) {
	case LDB_REPLY_ENTRY:
	{
		struct ldb_message *msg = ares->message;
		ent_r = ldapsrv_init_reply(call, LDAP_TAG_SearchResultEntry);
		if (ent_r == NULL) {
			return ldb_oom(ldb);
		}

		ctx->count++;

		/*
		 * Put the LDAP search response data under ent_r->msg
		 * so we can free that later once encoded
		 */
		talloc_steal(ent_r->msg, msg);

		ent = &ent_r->msg->r.SearchResultEntry;
		ent->dn = ldb_dn_get_extended_linearized(ent_r, msg->dn,
							 ctx->extended_type);
		ent->num_attributes = 0;
		ent->attributes = NULL;
		if (msg->num_elements == 0) {
			goto queue_reply;
		}
		ent->num_attributes = msg->num_elements;
		ent->attributes = talloc_array(ent_r, struct ldb_message_element, ent->num_attributes);
		if (ent->attributes == NULL) {
			return ldb_oom(ldb);
		}

		for (j=0; j < ent->num_attributes; j++) {
			ent->attributes[j].name = msg->elements[j].name;
			ent->attributes[j].num_values = 0;
			ent->attributes[j].values = NULL;
			if (ctx->attributesonly && (msg->elements[j].num_values == 0)) {
				continue;
			}
			ent->attributes[j].num_values = msg->elements[j].num_values;
			ent->attributes[j].values = msg->elements[j].values;
		}
queue_reply:
		status = ldapsrv_queue_reply(call, ent_r);
		if (NT_STATUS_EQUAL(status, NT_STATUS_FILE_TOO_LARGE)) {
			ret = ldb_request_done(req,
					       LDB_ERR_SIZE_LIMIT_EXCEEDED);
			ldb_asprintf_errstring(ldb,
					       "LDAP search response size "
					       "limited to %zu bytes\n",
					       LDAP_SERVER_MAX_REPLY_SIZE);
		} else if (!NT_STATUS_IS_OK(status)) {
			ret = ldb_request_done(req,
					       ldb_operr(ldb));
		} else {
			ret = LDB_SUCCESS;
		}
		break;
	}
	case LDB_REPLY_REFERRAL:
	{
		struct ldap_SearchResRef *ent_ref;

		/*
		 * TODO: This should be handled by the notification
		 * module not here
		 */
		if (call->notification.busy) {
			ret = LDB_SUCCESS;
			break;
		}

		ent_r = ldapsrv_init_reply(call, LDAP_TAG_SearchResultReference);
		if (ent_r == NULL) {
			return ldb_oom(ldb);
		}

		/*
		 * Put the LDAP referral data under ent_r->msg
		 * so we can free that later once encoded
		 */
		talloc_steal(ent_r->msg, ares->referral);

		ent_ref = &ent_r->msg->r.SearchResultReference;
		ent_ref->referral = ares->referral;

		status = ldapsrv_queue_reply(call, ent_r);
		if (!NT_STATUS_IS_OK(status)) {
			ret = LDB_ERR_OPERATIONS_ERROR;
		} else {
			ret = LDB_SUCCESS;
		}
		break;
	}
	case LDB_REPLY_DONE:
	{
		/*
		 * We don't queue the reply for this one, we let that
		 * happen outside
		 */
		ctx->controls = talloc_move(ctx, &ares->controls);

		TALLOC_FREE(ares);
		return ldb_request_done(req, LDB_SUCCESS);
	}
	default:
		/* Doesn't happen */
		ret = LDB_ERR_OPERATIONS_ERROR;
	}
	TALLOC_FREE(ares);

	return ret;
}