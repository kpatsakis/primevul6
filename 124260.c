static NTSTATUS ldapsrv_DelRequest(struct ldapsrv_call *call)
{
	struct ldap_DelRequest *req = &call->request->r.DelRequest;
	struct ldap_Result *del_result;
	struct ldapsrv_reply *del_reply;
	TALLOC_CTX *local_ctx;
	struct ldb_context *samdb = call->conn->ldb;
	struct ldb_dn *dn;
	const char *errstr = NULL;
	int result = LDAP_SUCCESS;
	int ldb_ret;
	struct ldb_result *res = NULL;

	DEBUG(10, ("DelRequest"));
	DEBUGADD(10, (" dn: %s\n", req->dn));

	local_ctx = talloc_named(call, 0, "DelRequest local memory context");
	NT_STATUS_HAVE_NO_MEMORY(local_ctx);

	dn = ldb_dn_new(local_ctx, samdb, req->dn);
	NT_STATUS_HAVE_NO_MEMORY(dn);

	DEBUG(10, ("DelRequest: dn: [%s]\n", req->dn));

	del_reply = ldapsrv_init_reply(call, LDAP_TAG_DelResponse);
	NT_STATUS_HAVE_NO_MEMORY(del_reply);

	if (result == LDAP_SUCCESS) {
		res = talloc_zero(local_ctx, struct ldb_result);
		NT_STATUS_HAVE_NO_MEMORY(res);
		ldb_ret = ldapsrv_del_with_controls(call, dn, call->request->controls, res);
		result = map_ldb_error(local_ctx, ldb_ret, ldb_errstring(samdb),
				       &errstr);
	}

	del_result = &del_reply->msg->r.DelResponse;
	del_result->dn = NULL;
	if ((res != NULL) && (res->refs != NULL)) {
		del_result->resultcode = map_ldb_error(local_ctx,
						       LDB_ERR_REFERRAL, NULL,
						       &errstr);
		del_result->errormessage = (errstr?talloc_strdup(del_reply,errstr):NULL);
		del_result->referral = talloc_strdup(call, *res->refs);
	} else {
		del_result->resultcode = result;
		del_result->errormessage = (errstr?talloc_strdup(del_reply,errstr):NULL);
		del_result->referral = NULL;
	}

	talloc_free(local_ctx);

	return ldapsrv_queue_reply(call, del_reply);
}