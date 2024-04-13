read_signature(cdk_stream_t inp, size_t pktlen, cdk_pkt_signature_t sig)
{
	size_t nbytes;
	size_t i, nsig;
	ssize_t size;
	cdk_error_t rc;

	if (!inp || !sig)
		return gnutls_assert_val(CDK_Inv_Value);

	if (DEBUG_PKT)
		_gnutls_write_log("read_signature: %d octets\n",
				  (int) pktlen);

	if (pktlen < 16)
		return gnutls_assert_val(CDK_Inv_Packet);
	sig->version = cdk_stream_getc(inp);
	if (sig->version < 2 || sig->version > 4)
		return gnutls_assert_val(CDK_Inv_Packet_Ver);

	sig->flags.exportable = 1;
	sig->flags.revocable = 1;

	if (sig->version < 4) {
		if (cdk_stream_getc(inp) != 5)
			return gnutls_assert_val(CDK_Inv_Packet);
		sig->sig_class = cdk_stream_getc(inp);
		sig->timestamp = read_32(inp);
		sig->keyid[0] = read_32(inp);
		sig->keyid[1] = read_32(inp);
		sig->pubkey_algo =
		    _pgp_pub_algo_to_cdk(cdk_stream_getc(inp));
		sig->digest_algo =
		    _pgp_hash_algo_to_gnutls(cdk_stream_getc(inp));
		sig->digest_start[0] = cdk_stream_getc(inp);
		sig->digest_start[1] = cdk_stream_getc(inp);
		nsig = cdk_pk_get_nsig(sig->pubkey_algo);
		if (!nsig)
			return gnutls_assert_val(CDK_Inv_Algo);
		for (i = 0; i < nsig; i++) {
			rc = read_mpi(inp, &sig->mpi[i], 0);
			if (rc)
				return gnutls_assert_val(rc);
		}
	} else {
		sig->sig_class = cdk_stream_getc(inp);
		sig->pubkey_algo =
		    _pgp_pub_algo_to_cdk(cdk_stream_getc(inp));
		sig->digest_algo =
		    _pgp_hash_algo_to_gnutls(cdk_stream_getc(inp));
		sig->hashed_size = read_16(inp);
		size = sig->hashed_size;
		sig->hashed = NULL;
		while (size > 0) {
			rc = read_subpkt(inp, &sig->hashed, &nbytes);
			if (rc)
				return gnutls_assert_val(rc);
			size -= nbytes;
		}
		sig->unhashed_size = read_16(inp);
		size = sig->unhashed_size;
		sig->unhashed = NULL;
		while (size > 0) {
			rc = read_subpkt(inp, &sig->unhashed, &nbytes);
			if (rc)
				return gnutls_assert_val(rc);
			size -= nbytes;
		}

		rc = parse_sig_subpackets(sig);
		if (rc)
			return gnutls_assert_val(rc);

		sig->digest_start[0] = cdk_stream_getc(inp);
		sig->digest_start[1] = cdk_stream_getc(inp);
		nsig = cdk_pk_get_nsig(sig->pubkey_algo);
		if (!nsig)
			return gnutls_assert_val(CDK_Inv_Algo);
		for (i = 0; i < nsig; i++) {
			rc = read_mpi(inp, &sig->mpi[i], 0);
			if (rc)
				return gnutls_assert_val(rc);
		}
	}

	return 0;
}