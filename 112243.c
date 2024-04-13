read_onepass_sig(cdk_stream_t inp, size_t pktlen,
		 cdk_pkt_onepass_sig_t sig)
{
	if (!inp || !sig)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_onepass_sig: %d octets\n",
				  (int) pktlen);

	if (pktlen != 13)
		return CDK_Inv_Packet;
	sig->version = cdk_stream_getc(inp);
	if (sig->version != 3)
		return CDK_Inv_Packet_Ver;
	sig->sig_class = cdk_stream_getc(inp);
	sig->digest_algo = _pgp_hash_algo_to_gnutls(cdk_stream_getc(inp));
	sig->pubkey_algo = _pgp_pub_algo_to_cdk(cdk_stream_getc(inp));
	sig->keyid[0] = read_32(inp);
	sig->keyid[1] = read_32(inp);
	sig->last = cdk_stream_getc(inp);
	return 0;
}