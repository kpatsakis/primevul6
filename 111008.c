static Bool av1_is_obu_frame(AV1State *state, ObuType obu_type)
{
	switch (obu_type) {
	case OBU_PADDING:
	case OBU_REDUNDANT_FRAME_HEADER:
		return GF_FALSE;
	case OBU_TEMPORAL_DELIMITER:
		return state->keep_temporal_delim ? GF_TRUE : GF_FALSE;
	default:
		return GF_TRUE;
	}
}