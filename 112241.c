read_pubkey_enc(cdk_stream_t inp, size_t pktlen, cdk_pkt_pubkey_enc_t pke)
{
	size_t i, nenc;

	if (!inp || !pke)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_pubkey_enc: %d octets\n",
				  (int) pktlen);

	if (pktlen < 12)
		return CDK_Inv_Packet;
	pke->version = cdk_stream_getc(inp);
	if (pke->version < 2 || pke->version > 3)
		return CDK_Inv_Packet;
	pke->keyid[0] = read_32(inp);
	pke->keyid[1] = read_32(inp);
	if (!pke->keyid[0] && !pke->keyid[1])
		pke->throw_keyid = 1;	/* RFC2440 "speculative" keyID */
	pke->pubkey_algo = _pgp_pub_algo_to_cdk(cdk_stream_getc(inp));
	nenc = cdk_pk_get_nenc(pke->pubkey_algo);
	if (!nenc)
		return CDK_Inv_Algo;
	for (i = 0; i < nenc; i++) {
		cdk_error_t rc = read_mpi(inp, &pke->mpi[i], 0);
		if (rc)
			return gnutls_assert_val(rc);
	}

	return 0;
}