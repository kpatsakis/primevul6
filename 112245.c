read_user_id(cdk_stream_t inp, size_t pktlen, cdk_pkt_userid_t user_id)
{
	size_t nread;
	cdk_error_t rc;

	if (!inp || !user_id)
		return CDK_Inv_Value;
	if (!pktlen)
		return CDK_Inv_Packet;

	if (DEBUG_PKT)
		_gnutls_write_log("read_user_id: %lu octets\n",
				  (unsigned long) pktlen);

	user_id->len = pktlen;
	rc = stream_read(inp, user_id->name, pktlen, &nread);
	if (rc)
		return rc;
	if (nread != pktlen)
		return CDK_Inv_Packet;
	user_id->name[nread] = '\0';
	return rc;
}