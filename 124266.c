NTSTATUS ldapsrv_do_call(struct ldapsrv_call *call)
{
	unsigned int i;
	struct ldap_message *msg = call->request;
	struct ldapsrv_connection *conn = call->conn;
	NTSTATUS status;
	bool expired;

	expired = timeval_expired(&conn->limits.expire_time);
	if (expired) {
		status = ldapsrv_expired(call);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
		return NT_STATUS_NETWORK_SESSION_EXPIRED;
	}

	/* Check for undecoded critical extensions */
	for (i=0; msg->controls && msg->controls[i]; i++) {
		if (!msg->controls_decoded[i] && 
		    msg->controls[i]->critical) {
			DEBUG(3, ("ldapsrv_do_call: Critical extension %s is not known to this server\n",
				  msg->controls[i]->oid));
			return ldapsrv_unwilling(call, LDAP_UNAVAILABLE_CRITICAL_EXTENSION);
		}
	}

	if (call->conn->authz_logged == false) {
		bool log = true;

		/*
		 * We do not want to log anonymous access if the query
		 * is just for the rootDSE, or it is a startTLS or a
		 * Bind.
		 *
		 * A rootDSE search could also be done over
		 * CLDAP anonymously for example, so these don't
		 * really count.
		 * Essentially we want to know about
		 * access beyond that normally done prior to a
		 * bind.
		 */

		switch(call->request->type) {
		case LDAP_TAG_BindRequest:
		case LDAP_TAG_UnbindRequest:
		case LDAP_TAG_AbandonRequest:
			log = false;
			break;
		case LDAP_TAG_ExtendedResponse: {
			struct ldap_ExtendedRequest *req = &call->request->r.ExtendedRequest;
			if (strcmp(req->oid, LDB_EXTENDED_START_TLS_OID) == 0) {
				log = false;
			}
			break;
		}
		case LDAP_TAG_SearchRequest: {
			struct ldap_SearchRequest *req = &call->request->r.SearchRequest;
			if (req->scope == LDAP_SEARCH_SCOPE_BASE) {
				if (req->basedn[0] == '\0') {
					log = false;
				}
			}
			break;
		}
		default:
			break;
		}

		if (log) {
			const char *transport_protection = AUTHZ_TRANSPORT_PROTECTION_NONE;
			if (call->conn->sockets.active == call->conn->sockets.tls) {
				transport_protection = AUTHZ_TRANSPORT_PROTECTION_TLS;
			}

			log_successful_authz_event(call->conn->connection->msg_ctx,
						   call->conn->connection->lp_ctx,
						   call->conn->connection->remote_address,
						   call->conn->connection->local_address,
						   "LDAP",
						   "no bind",
						   transport_protection,
						   call->conn->session_info);

			call->conn->authz_logged = true;
		}
	}

	switch(call->request->type) {
	case LDAP_TAG_BindRequest:
		return ldapsrv_BindRequest(call);
	case LDAP_TAG_UnbindRequest:
		return ldapsrv_UnbindRequest(call);
	case LDAP_TAG_SearchRequest:
		return ldapsrv_SearchRequest(call);
	case LDAP_TAG_ModifyRequest:
		status = ldapsrv_ModifyRequest(call);
		break;
	case LDAP_TAG_AddRequest:
		status = ldapsrv_AddRequest(call);
		break;
	case LDAP_TAG_DelRequest:
		status = ldapsrv_DelRequest(call);
		break;
	case LDAP_TAG_ModifyDNRequest:
		status = ldapsrv_ModifyDNRequest(call);
		break;
	case LDAP_TAG_CompareRequest:
		return ldapsrv_CompareRequest(call);
	case LDAP_TAG_AbandonRequest:
		return ldapsrv_AbandonRequest(call);
	case LDAP_TAG_ExtendedRequest:
		status = ldapsrv_ExtendedRequest(call);
		break;
	default:
		return ldapsrv_unwilling(call, LDAP_PROTOCOL_ERROR);
	}

	if (NT_STATUS_IS_OK(status)) {
		ldapsrv_notification_retry_setup(call->conn->service, true);
	}

	return status;
}