Bool gf_ac3_parser(u8 *buf, u32 buflen, u32 *pos, GF_AC3Config *hdr, Bool full_parse)
{
	GF_BitStream *bs;
	Bool ret;

	if (buflen < 6) return GF_FALSE;
	(*pos) = AC3_FindSyncCode(buf, buflen);
	if (*pos >= buflen) return GF_FALSE;

	bs = gf_bs_new((const char*)(buf + *pos), buflen, GF_BITSTREAM_READ);
	ret = gf_ac3_parser_bs(bs, hdr, full_parse);
	gf_bs_del(bs);

	return ret;
}