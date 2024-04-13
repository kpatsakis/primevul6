static GF_Err av1_parse_frame(GF_BitStream *bs, AV1State *state, u64 obu_start, u64 obu_size)
{
	av1_parse_frame_header(bs, state);
	//byte alignment
	gf_bs_align(bs);
	return av1_parse_tile_group(bs, state, obu_start, obu_size);
}