static NTSTATUS ldapsrv_encode(TALLOC_CTX *mem_ctx,
			       struct ldapsrv_reply *reply)
{
	bool bret = ldap_encode(reply->msg,
				samba_ldap_control_handlers(),
				&reply->blob,
				mem_ctx);
	TALLOC_FREE(reply->msg);
	if (!bret) {
		DEBUG(0,("Failed to encode ldap reply of type %d: "
			 "ldap_encode() failed\n",
			 reply->msg->type));
		return NT_STATUS_NO_MEMORY;
	}

	talloc_set_name_const(reply->blob.data,
			      "Outgoing, encoded single LDAP reply");

	return NT_STATUS_OK;
}