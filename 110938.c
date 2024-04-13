GF_Err aom_av1_parse_temporal_unit_from_ivf(GF_BitStream *bs, AV1State *state)
{
	u64 frame_size, pts_ignored;
	GF_Err e;
	if (gf_bs_available(bs)<12) return GF_EOS;
	e = gf_media_parse_ivf_frame_header(bs, &frame_size, &pts_ignored);
	if (e) return e;
	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[AV1] IVF frame detected (size "LLU")\n", frame_size));

	if (gf_bs_available(bs) < frame_size) return GF_EOS;

	while (frame_size > 0) {
		u64 obu_size = 0, pos = gf_bs_get_position(bs);

		e = gf_av1_parse_obu(bs, &state->obu_type, &obu_size, NULL, state);
		if (e != GF_OK)
			return e;

		if (obu_size != gf_bs_get_position(bs) - pos) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[AV1] IVF frame size "LLU" different from consumed bytes "LLU".\n", obu_size, gf_bs_get_position(bs) - pos));
			return GF_NON_COMPLIANT_BITSTREAM;
		}

		av1_populate_state_from_obu(bs, pos, obu_size, state->obu_type, state);

		frame_size -= obu_size;
	}
	return GF_OK;
}