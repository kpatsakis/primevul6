read_literal(cdk_stream_t inp, size_t pktlen,
	     cdk_pkt_literal_t * ret_pt, int is_partial)
{
	cdk_pkt_literal_t pt = *ret_pt;
	size_t nread;
	cdk_error_t rc;

	if (!inp || !pt)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_literal: %d octets\n",
				  (int) pktlen);

	pt->mode = cdk_stream_getc(inp);
	if (pt->mode != 0x62 && pt->mode != 0x74 && pt->mode != 0x75)
		return CDK_Inv_Packet;
	if (cdk_stream_eof(inp))
		return CDK_Inv_Packet;

	pt->namelen = cdk_stream_getc(inp);
	if (pt->namelen > 0) {
		*ret_pt = pt =
		    cdk_realloc(pt, sizeof *pt + pt->namelen + 2);
		if (!pt)
			return CDK_Out_Of_Core;
		pt->name = (char *) pt + sizeof(*pt);
		rc = stream_read(inp, pt->name, pt->namelen, &nread);
		if (rc)
			return rc;
		if ((int) nread != pt->namelen)
			return CDK_Inv_Packet;
		pt->name[pt->namelen] = '\0';
	}
	pt->timestamp = read_32(inp);
	pktlen = pktlen - 6 - pt->namelen;
	if (is_partial)
		_cdk_stream_set_blockmode(inp, pktlen);
	pt->buf = inp;
	pt->len = pktlen;
	return 0;
}