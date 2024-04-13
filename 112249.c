read_public_key(cdk_stream_t inp, size_t pktlen, cdk_pkt_pubkey_t pk)
{
	size_t i, ndays, npkey;

	if (!inp || !pk)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_public_key: %d octets\n",
				  (int) pktlen);

	pk->is_invalid = 1;	/* default to detect missing self signatures */
	pk->is_revoked = 0;
	pk->has_expired = 0;

	pk->version = cdk_stream_getc(inp);
	if (pk->version < 2 || pk->version > 4)
		return CDK_Inv_Packet_Ver;
	pk->timestamp = read_32(inp);
	if (pk->version < 4) {
		ndays = read_16(inp);
		if (ndays)
			pk->expiredate = pk->timestamp + ndays * 86400L;
	}

	pk->pubkey_algo = _pgp_pub_algo_to_cdk(cdk_stream_getc(inp));
	npkey = cdk_pk_get_npkey(pk->pubkey_algo);
	if (!npkey) {
		gnutls_assert();
		_gnutls_write_log("invalid public key algorithm %d\n",
				  pk->pubkey_algo);
		return CDK_Inv_Algo;
	}
	for (i = 0; i < npkey; i++) {
		cdk_error_t rc = read_mpi(inp, &pk->mpi[i], 0);
		if (rc)
			return gnutls_assert_val(rc);
	}

	/* This value is just for the first run and will be
	   replaced with the actual key flags from the self signature. */
	pk->pubkey_usage = 0;
	return 0;
}