void gf_bs_get_content(GF_BitStream *bs, u8 **output, u32 *outSize)
{
	gf_bs_get_content_no_truncate(bs, output, outSize, NULL);
}