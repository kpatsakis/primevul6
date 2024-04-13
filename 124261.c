static int ldapsrv_rename_with_controls(struct ldapsrv_call *call,
					struct ldb_dn *olddn,
					struct ldb_dn *newdn,
					struct ldb_control **controls,
					struct ldb_result *res)
{
	struct ldb_context *ldb = call->conn->ldb;
	struct ldb_request *req;
	int ret;

	ret = ldb_build_rename_req(&req, ldb, ldb,
					olddn,
					newdn,
					controls,
					res,
					ldb_modify_default_callback,
					NULL);

	if (ret != LDB_SUCCESS) return ret;

	if (call->conn->global_catalog) {
		return ldb_error(ldb, LDB_ERR_UNWILLING_TO_PERFORM, "modify forbidden on global catalog port");
	}
	ldb_request_add_control(req, DSDB_CONTROL_NO_GLOBAL_CATALOG, false, NULL);

	ret = ldb_transaction_start(ldb);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (!call->conn->is_privileged) {
		ldb_req_mark_untrusted(req);
	}

	LDB_REQ_SET_LOCATION(req);

	ret = ldb_request(ldb, req);
	if (ret == LDB_SUCCESS) {
		ret = ldb_wait(req->handle, LDB_WAIT_ALL);
	}

	if (ret == LDB_SUCCESS) {
		ret = ldb_transaction_commit(ldb);
	}
	else {
		ldb_transaction_cancel(ldb);
	}

	talloc_free(req);
	return ret;
}