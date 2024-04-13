int ldapsrv_backend_Init(struct ldapsrv_connection *conn,
			      char **errstring)
{
	int ret = samdb_connect_url(conn,
				    conn->connection->event.ctx,
				    conn->lp_ctx,
				    conn->session_info,
				    conn->global_catalog ? LDB_FLG_RDONLY : 0,
				    "sam.ldb",
				    conn->connection->remote_address,
				    &conn->ldb,
				    errstring);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	if (conn->server_credentials) {
		struct gensec_security *gensec_security = NULL;
		const char **sasl_mechs = NULL;
		NTSTATUS status;

		status = samba_server_gensec_start(conn,
						   conn->connection->event.ctx,
						   conn->connection->msg_ctx,
						   conn->lp_ctx,
						   conn->server_credentials,
						   "ldap",
						   &gensec_security);
		if (!NT_STATUS_IS_OK(status)) {
			DBG_ERR("samba_server_gensec_start failed: %s\n",
				nt_errstr(status));
			return LDB_ERR_OPERATIONS_ERROR;
		}

		/* ldb can have a different lifetime to conn, so we
		   need to ensure that sasl_mechs lives as long as the
		   ldb does */
		sasl_mechs = gensec_security_sasl_names(gensec_security,
							conn->ldb);
		TALLOC_FREE(gensec_security);
		if (sasl_mechs == NULL) {
			DBG_ERR("Failed to get sasl mechs!\n");
			return LDB_ERR_OPERATIONS_ERROR;
		}

		ldb_set_opaque(conn->ldb, "supportedSASLMechanisms", sasl_mechs);
	}

	return LDB_SUCCESS;
}