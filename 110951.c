u32 gf_media_nalu_next_start_code_bs(GF_BitStream *bs)
{
	return gf_media_nalu_locate_start_code_bs(bs, 0);
}