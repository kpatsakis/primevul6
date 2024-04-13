static NTSTATUS ldapsrv_ModifyDNRequest(struct ldapsrv_call *call)
{
	struct ldap_ModifyDNRequest *req = &call->request->r.ModifyDNRequest;
	struct ldap_Result *modifydn;
	struct ldapsrv_reply *modifydn_r;
	TALLOC_CTX *local_ctx;
	struct ldb_context *samdb = call->conn->ldb;
	struct ldb_dn *olddn, *newdn=NULL, *newrdn;
	struct ldb_dn *parentdn = NULL;
	const char *errstr = NULL;
	int result = LDAP_SUCCESS;
	int ldb_ret;
	struct ldb_result *res = NULL;

	DEBUG(10, ("ModifyDNRequest"));
	DEBUGADD(10, (" dn: %s", req->dn));
	DEBUGADD(10, (" newrdn: %s\n", req->newrdn));

	local_ctx = talloc_named(call, 0, "ModifyDNRequest local memory context");
	NT_STATUS_HAVE_NO_MEMORY(local_ctx);

	olddn = ldb_dn_new(local_ctx, samdb, req->dn);
	NT_STATUS_HAVE_NO_MEMORY(olddn);

	newrdn = ldb_dn_new(local_ctx, samdb, req->newrdn);
	NT_STATUS_HAVE_NO_MEMORY(newrdn);

	DEBUG(10, ("ModifyDNRequest: olddn: [%s]\n", req->dn));
	DEBUG(10, ("ModifyDNRequest: newrdn: [%s]\n", req->newrdn));

	if (ldb_dn_get_comp_num(newrdn) == 0) {
		result = LDAP_PROTOCOL_ERROR;
		map_ldb_error(local_ctx, LDB_ERR_PROTOCOL_ERROR, NULL,
			      &errstr);
		goto reply;
	}

	if (ldb_dn_get_comp_num(newrdn) > 1) {
		result = LDAP_NAMING_VIOLATION;
		map_ldb_error(local_ctx, LDB_ERR_NAMING_VIOLATION, NULL,
			      &errstr);
		goto reply;
	}

	/* we can't handle the rename if we should not remove the old dn */
	if (!req->deleteolddn) {
		result = LDAP_UNWILLING_TO_PERFORM;
		map_ldb_error(local_ctx, LDB_ERR_UNWILLING_TO_PERFORM, NULL,
			      &errstr);
		errstr = talloc_asprintf(local_ctx,
			"%s. Old RDN must be deleted", errstr);
		goto reply;
	}

	if (req->newsuperior) {
		DEBUG(10, ("ModifyDNRequest: newsuperior: [%s]\n", req->newsuperior));
		parentdn = ldb_dn_new(local_ctx, samdb, req->newsuperior);
	}

	if (!parentdn) {
		parentdn = ldb_dn_get_parent(local_ctx, olddn);
	}
	if (!parentdn) {
		result = LDAP_NO_SUCH_OBJECT;
		map_ldb_error(local_ctx, LDB_ERR_NO_SUCH_OBJECT, NULL, &errstr);
		goto reply;
	}

	if ( ! ldb_dn_add_child(parentdn, newrdn)) {
		result = LDAP_OTHER;
		map_ldb_error(local_ctx, LDB_ERR_OTHER, NULL, &errstr);
		goto reply;
	}
	newdn = parentdn;

reply:
	modifydn_r = ldapsrv_init_reply(call, LDAP_TAG_ModifyDNResponse);
	NT_STATUS_HAVE_NO_MEMORY(modifydn_r);

	if (result == LDAP_SUCCESS) {
		res = talloc_zero(local_ctx, struct ldb_result);
		NT_STATUS_HAVE_NO_MEMORY(res);
		ldb_ret = ldapsrv_rename_with_controls(call, olddn, newdn, call->request->controls, res);
		result = map_ldb_error(local_ctx, ldb_ret, ldb_errstring(samdb),
				       &errstr);
	}

	modifydn = &modifydn_r->msg->r.ModifyDNResponse;
	modifydn->dn = NULL;
	if ((res != NULL) && (res->refs != NULL)) {
		modifydn->resultcode = map_ldb_error(local_ctx,
						     LDB_ERR_REFERRAL, NULL,
						     &errstr);;
		modifydn->errormessage = (errstr?talloc_strdup(modifydn_r,errstr):NULL);
		modifydn->referral = talloc_strdup(call, *res->refs);
	} else {
		modifydn->resultcode = result;
		modifydn->errormessage = (errstr?talloc_strdup(modifydn_r,errstr):NULL);
		modifydn->referral = NULL;
	}

	talloc_free(local_ctx);

	return ldapsrv_queue_reply(call, modifydn_r);
}