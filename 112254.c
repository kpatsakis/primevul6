read_secret_subkey(cdk_stream_t inp, size_t pktlen, cdk_pkt_seckey_t sk)
{
	cdk_error_t rc;

	if (!inp || !sk || !sk->pk)
		return CDK_Inv_Value;

	rc = read_secret_key(inp, pktlen, sk);
	sk->is_primary = 0;
	return rc;
}