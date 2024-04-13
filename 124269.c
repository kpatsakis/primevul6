static NTSTATUS ldapsrv_ModifyRequest(struct ldapsrv_call *call)
{
	struct ldap_ModifyRequest *req = &call->request->r.ModifyRequest;
	struct ldap_Result *modify_result;
	struct ldapsrv_reply *modify_reply;
	TALLOC_CTX *local_ctx;
	struct ldb_context *samdb = call->conn->ldb;
	struct ldb_message *msg = NULL;
	struct ldb_dn *dn;
	const char *errstr = NULL;
	int result = LDAP_SUCCESS;
	int ldb_ret;
	unsigned int i,j;
	struct ldb_result *res = NULL;

	DEBUG(10, ("ModifyRequest"));
	DEBUGADD(10, (" dn: %s\n", req->dn));

	local_ctx = talloc_named(call, 0, "ModifyRequest local memory context");
	NT_STATUS_HAVE_NO_MEMORY(local_ctx);

	dn = ldb_dn_new(local_ctx, samdb, req->dn);
	NT_STATUS_HAVE_NO_MEMORY(dn);

	DEBUG(10, ("ModifyRequest: dn: [%s]\n", req->dn));

	msg = ldb_msg_new(local_ctx);
	NT_STATUS_HAVE_NO_MEMORY(msg);

	msg->dn = dn;

	if (req->num_mods > 0) {
		msg->num_elements = req->num_mods;
		msg->elements = talloc_array(msg, struct ldb_message_element, req->num_mods);
		NT_STATUS_HAVE_NO_MEMORY(msg->elements);

		for (i=0; i < msg->num_elements; i++) {
			msg->elements[i].name = discard_const_p(char, req->mods[i].attrib.name);
			msg->elements[i].num_values = 0;
			msg->elements[i].values = NULL;

			switch (req->mods[i].type) {
			default:
				result = LDAP_PROTOCOL_ERROR;
				map_ldb_error(local_ctx,
					LDB_ERR_PROTOCOL_ERROR, NULL, &errstr);
				errstr = talloc_asprintf(local_ctx,
					"%s. Invalid LDAP_MODIFY_* type", errstr);
				goto reply;
			case LDAP_MODIFY_ADD:
				msg->elements[i].flags = LDB_FLAG_MOD_ADD;
				break;
			case LDAP_MODIFY_DELETE:
				msg->elements[i].flags = LDB_FLAG_MOD_DELETE;
				break;
			case LDAP_MODIFY_REPLACE:
				msg->elements[i].flags = LDB_FLAG_MOD_REPLACE;
				break;
			}

			msg->elements[i].num_values = req->mods[i].attrib.num_values;
			if (msg->elements[i].num_values > 0) {
				msg->elements[i].values = talloc_array(msg->elements, struct ldb_val,
								       msg->elements[i].num_values);
				NT_STATUS_HAVE_NO_MEMORY(msg->elements[i].values);

				for (j=0; j < msg->elements[i].num_values; j++) {
					msg->elements[i].values[j].length = req->mods[i].attrib.values[j].length;
					msg->elements[i].values[j].data = req->mods[i].attrib.values[j].data;			
				}
			}
		}
	}

reply:
	modify_reply = ldapsrv_init_reply(call, LDAP_TAG_ModifyResponse);
	NT_STATUS_HAVE_NO_MEMORY(modify_reply);

	if (result == LDAP_SUCCESS) {
		res = talloc_zero(local_ctx, struct ldb_result);
		NT_STATUS_HAVE_NO_MEMORY(res);
		ldb_ret = ldapsrv_mod_with_controls(call, msg, call->request->controls, res);
		result = map_ldb_error(local_ctx, ldb_ret, ldb_errstring(samdb),
				       &errstr);
	}

	modify_result = &modify_reply->msg->r.ModifyResponse;
	modify_result->dn = NULL;
	if ((res != NULL) && (res->refs != NULL)) {
		modify_result->resultcode = map_ldb_error(local_ctx,
							  LDB_ERR_REFERRAL,
							  NULL, &errstr);
		modify_result->errormessage = (errstr?talloc_strdup(modify_reply, errstr):NULL);
		modify_result->referral = talloc_strdup(call, *res->refs);
	} else {
		modify_result->resultcode = result;
		modify_result->errormessage = (errstr?talloc_strdup(modify_reply, errstr):NULL);
		modify_result->referral = NULL;
	}
	talloc_free(local_ctx);

	return ldapsrv_queue_reply(call, modify_reply);

}