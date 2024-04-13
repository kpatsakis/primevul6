static NTSTATUS ldapsrv_CompareRequest(struct ldapsrv_call *call)
{
	struct ldap_CompareRequest *req = &call->request->r.CompareRequest;
	struct ldap_Result *compare;
	struct ldapsrv_reply *compare_r;
	TALLOC_CTX *local_ctx;
	struct ldb_context *samdb = call->conn->ldb;
	struct ldb_result *res = NULL;
	struct ldb_dn *dn;
	const char *attrs[1];
	const char *errstr = NULL;
	const char *filter = NULL;
	int result = LDAP_SUCCESS;
	int ldb_ret;

	DEBUG(10, ("CompareRequest"));
	DEBUGADD(10, (" dn: %s\n", req->dn));

	local_ctx = talloc_named(call, 0, "CompareRequest local_memory_context");
	NT_STATUS_HAVE_NO_MEMORY(local_ctx);

	dn = ldb_dn_new(local_ctx, samdb, req->dn);
	NT_STATUS_HAVE_NO_MEMORY(dn);

	DEBUG(10, ("CompareRequest: dn: [%s]\n", req->dn));
	filter = talloc_asprintf(local_ctx, "(%s=%*s)", req->attribute, 
				 (int)req->value.length, req->value.data);
	NT_STATUS_HAVE_NO_MEMORY(filter);

	DEBUGADD(10, ("CompareRequest: attribute: [%s]\n", filter));

	attrs[0] = NULL;

	compare_r = ldapsrv_init_reply(call, LDAP_TAG_CompareResponse);
	NT_STATUS_HAVE_NO_MEMORY(compare_r);

	if (result == LDAP_SUCCESS) {
		ldb_ret = ldb_search(samdb, local_ctx, &res,
				     dn, LDB_SCOPE_BASE, attrs, "%s", filter);
		if (ldb_ret != LDB_SUCCESS) {
			result = map_ldb_error(local_ctx, ldb_ret,
					       ldb_errstring(samdb), &errstr);
			DEBUG(10,("CompareRequest: error: %s\n", errstr));
		} else if (res->count == 0) {
			DEBUG(10,("CompareRequest: doesn't matched\n"));
			result = LDAP_COMPARE_FALSE;
			errstr = NULL;
		} else if (res->count == 1) {
			DEBUG(10,("CompareRequest: matched\n"));
			result = LDAP_COMPARE_TRUE;
			errstr = NULL;
		} else if (res->count > 1) {
			result = LDAP_OTHER;
			map_ldb_error(local_ctx, LDB_ERR_OTHER, NULL, &errstr);
			errstr = talloc_asprintf(local_ctx,
				"%s. Too many objects match!", errstr);
			DEBUG(10,("CompareRequest: %d results: %s\n", res->count, errstr));
		}
	}

	compare = &compare_r->msg->r.CompareResponse;
	compare->dn = NULL;
	compare->resultcode = result;
	compare->errormessage = (errstr?talloc_strdup(compare_r,errstr):NULL);
	compare->referral = NULL;

	talloc_free(local_ctx);

	return ldapsrv_queue_reply(call, compare_r);
}