read_secret_key(cdk_stream_t inp, size_t pktlen, cdk_pkt_seckey_t sk)
{
	size_t p1, p2, nread;
	int i, nskey;
	int rc;

	if (!inp || !sk || !sk->pk)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_secret_key: %d octets\n",
				  (int) pktlen);

	p1 = cdk_stream_tell(inp);
	rc = read_public_key(inp, pktlen, sk->pk);
	if (rc)
		return rc;

	sk->s2k_usage = cdk_stream_getc(inp);
	sk->protect.sha1chk = 0;
	if (sk->s2k_usage == 254 || sk->s2k_usage == 255) {
		sk->protect.sha1chk = (sk->s2k_usage == 254);
		sk->protect.algo =
		    _pgp_cipher_to_gnutls(cdk_stream_getc(inp));
		if (sk->protect.algo == GNUTLS_CIPHER_UNKNOWN)
			return gnutls_assert_val(CDK_Inv_Algo);

		sk->protect.s2k = cdk_calloc(1, sizeof *sk->protect.s2k);
		if (!sk->protect.s2k)
			return CDK_Out_Of_Core;
		rc = read_s2k(inp, sk->protect.s2k);
		if (rc)
			return rc;
		/* refer to --export-secret-subkeys in gpg(1) */
		if (sk->protect.s2k->mode == CDK_S2K_GNU_EXT)
			sk->protect.ivlen = 0;
		else {
			sk->protect.ivlen =
			    gnutls_cipher_get_block_size(sk->protect.algo);
			if (!sk->protect.ivlen)
				return CDK_Inv_Packet;
			rc = stream_read(inp, sk->protect.iv,
					 sk->protect.ivlen, &nread);
			if (rc)
				return rc;
			if (nread != sk->protect.ivlen)
				return CDK_Inv_Packet;
		}
	} else
		sk->protect.algo = _pgp_cipher_to_gnutls(sk->s2k_usage);
	if (sk->protect.algo == GNUTLS_CIPHER_UNKNOWN)
		return gnutls_assert_val(CDK_Inv_Algo);
	else if (sk->protect.algo == GNUTLS_CIPHER_NULL) {
		sk->csum = 0;
		nskey = cdk_pk_get_nskey(sk->pk->pubkey_algo);
		if (!nskey) {
			gnutls_assert();
			return CDK_Inv_Algo;
		}
		for (i = 0; i < nskey; i++) {
			rc = read_mpi(inp, &sk->mpi[i], 1);
			if (rc)
				return gnutls_assert_val(rc);
		}
		sk->csum = read_16(inp);
		sk->is_protected = 0;
	} else if (sk->pk->version < 4) {
		/* The length of each multiprecision integer is stored in plaintext. */
		nskey = cdk_pk_get_nskey(sk->pk->pubkey_algo);
		if (!nskey) {
			gnutls_assert();
			return CDK_Inv_Algo;
		}
		for (i = 0; i < nskey; i++) {
			rc = read_mpi(inp, &sk->mpi[i], 1);
			if (rc)
				return gnutls_assert_val(rc);
		}
		sk->csum = read_16(inp);
		sk->is_protected = 1;
	} else {
		/* We need to read the rest of the packet because we do not
		   have any information how long the encrypted mpi's are */
		p2 = cdk_stream_tell(inp);
		p2 -= p1;
		sk->enclen = pktlen - p2;
		if (sk->enclen < 2)
			return CDK_Inv_Packet;	/* at least 16 bits for the checksum! */
		sk->encdata = cdk_calloc(1, sk->enclen + 1);
		if (!sk->encdata)
			return CDK_Out_Of_Core;
		if (stream_read(inp, sk->encdata, sk->enclen, &nread))
			return CDK_Inv_Packet;
		/* Handle the GNU S2K extensions we know (just gnu-dummy right now): */
		if (sk->protect.s2k->mode == CDK_S2K_GNU_EXT) {
			unsigned char gnumode;
			if ((sk->enclen < strlen("GNU") + 1) ||
			    (0 !=
			     memcmp("GNU", sk->encdata, strlen("GNU"))))
				return CDK_Inv_Packet;
			gnumode = sk->encdata[strlen("GNU")];
			/* we only handle gnu-dummy (mode 1).
			   mode 2 should refer to external smart cards.
			 */
			if (gnumode != 1)
				return CDK_Inv_Packet;
			/* gnu-dummy should have no more data */
			if (sk->enclen != strlen("GNU") + 1)
				return CDK_Inv_Packet;
		}
		nskey = cdk_pk_get_nskey(sk->pk->pubkey_algo);
		if (!nskey) {
			gnutls_assert();
			return CDK_Inv_Algo;
		}
		/* We mark each MPI entry with NULL to indicate a protected key. */
		for (i = 0; i < nskey; i++)
			sk->mpi[i] = NULL;
		sk->is_protected = 1;
	}

	sk->is_primary = 1;
	_cdk_copy_pk_to_sk(sk->pk, sk);
	return 0;
}