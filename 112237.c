read_public_subkey(cdk_stream_t inp, size_t pktlen, cdk_pkt_pubkey_t pk)
{
	if (!inp || !pk)
		return CDK_Inv_Value;
	return read_public_key(inp, pktlen, pk);
}