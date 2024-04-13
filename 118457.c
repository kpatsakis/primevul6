GF_Err gf_av1_parse_obu(GF_BitStream *bs, ObuType *obu_type, u64 *obu_size, u32 *obu_hdr_size, AV1State *state)
{
	GF_Err e = GF_OK;
	u32 i, hdr_size;
	u64 pos = gf_bs_get_position(bs);

	if (!bs || !obu_type || !state)
		return GF_BAD_PARAM;

	gf_bs_mark_overflow(bs, GF_TRUE);

	state->obu_extension_flag = state->obu_has_size_field = 0;
	state->temporal_id = state->spatial_id = 0;
	state->frame_state.uncompressed_header_bytes = 0;
	e = gf_av1_parse_obu_header(bs, obu_type, &state->obu_extension_flag, &state->obu_has_size_field, &state->temporal_id, &state->spatial_id);
	if (gf_bs_is_overflow(bs)) e = GF_NON_COMPLIANT_BITSTREAM;
	if (e)
		return e;

	//at this point obu_size is either zero or the size of the containing buffer (likely the Temporal Unit)
	if (state->obu_has_size_field) {
		*obu_size = (u32)gf_av1_leb128_read(bs, NULL);
	}
	else {
		if (*obu_size >= 1 + state->obu_extension_flag) {
			*obu_size = *obu_size - 1 - state->obu_extension_flag;
		}
		else {
			GF_LOG(state->config ? GF_LOG_WARNING : GF_LOG_DEBUG, GF_LOG_CODING, ("[AV1] computed OBU size "LLD" (input value = "LLU"). Skipping.\n", *obu_size - 1 - state->obu_extension_flag, *obu_size));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
	}
	hdr_size = (u32)(gf_bs_get_position(bs) - pos);
	if (gf_bs_is_overflow(bs) || (gf_bs_available(bs) < *obu_size) ) {
		gf_bs_seek(bs, pos);
		return GF_BUFFER_TOO_SMALL;
	}
	//gpac's internal obu_size includes the header + the payload
	*obu_size += hdr_size;
	if (obu_hdr_size) *obu_hdr_size = hdr_size;

	if (*obu_type != OBU_SEQUENCE_HEADER && *obu_type != OBU_TEMPORAL_DELIMITER &&
		state->OperatingPointIdc != 0 && state->obu_extension_flag == 1)
	{
		u32 inTemporalLayer = (state->OperatingPointIdc >> state->temporal_id) & 1;
		u32 inSpatialLayer = (state->OperatingPointIdc >> (state->spatial_id + 8)) & 1;
		if (!inTemporalLayer || !inSpatialLayer) {
			*obu_type = -1;
			gf_bs_seek(bs, pos + *obu_size);
			return GF_OK;
		}
	}

	e = GF_OK;

	/* for AVIF a1lx */
	for (i = state->spatial_id; i < 4; i++) {
		state->layer_size[i] = (u32) (pos + *obu_size);
	}

	switch (*obu_type) {
	case OBU_SEQUENCE_HEADER:
		av1_parse_sequence_header_obu(bs, state);
		if (gf_bs_is_overflow(bs) || (gf_bs_get_position(bs) > pos + *obu_size)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] Sequence header parsing consumed too many bytes !\n"));
			e = GF_NON_COMPLIANT_BITSTREAM;
		}
		gf_bs_seek(bs, pos + *obu_size);
		break;

	case OBU_METADATA:
		av1_parse_obu_metadata(state, bs);
		gf_bs_seek(bs, pos + *obu_size);
		if (gf_bs_is_overflow(bs)) e = GF_NON_COMPLIANT_BITSTREAM;
		break;

	case OBU_FRAME_HEADER:
	case OBU_REDUNDANT_FRAME_HEADER:
		if (state->config) {
			av1_parse_frame_header(bs, state);
			if (gf_bs_is_overflow(bs) || (gf_bs_get_position(bs) > pos + *obu_size)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] Frame header parsing consumed too many bytes !\n"));
				e = GF_NON_COMPLIANT_BITSTREAM;
			}
		}
		gf_bs_seek(bs, pos + *obu_size);
		break;
	case OBU_FRAME:
		e = av1_parse_frame(bs, state, pos, *obu_size);
		if (gf_bs_is_overflow(bs) || (gf_bs_get_position(bs) != pos + *obu_size)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] Frame parsing did not consume the right number of bytes !\n"));
			e = GF_NON_COMPLIANT_BITSTREAM;
		}
		gf_bs_seek(bs, pos + *obu_size);
		break;
	case OBU_TILE_GROUP:
		if (state->config) {
			e = av1_parse_tile_group(bs, state, pos, *obu_size);
			if (gf_bs_is_overflow(bs) || (gf_bs_get_position(bs) != pos + *obu_size)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] Tile group parsing did not consume the right number of bytes !\n"));
				e = GF_NON_COMPLIANT_BITSTREAM;
			}
		}
		gf_bs_seek(bs, pos + *obu_size);
		break;
	case OBU_TEMPORAL_DELIMITER:
		state->frame_state.seen_frame_header = GF_FALSE;
		state->clli_valid = state->mdcv_valid = 0;
	case OBU_PADDING:
		gf_bs_seek(bs, pos + *obu_size);
		break;
	default:
		GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[AV1] unknown OBU type %u (size "LLU"). Skipping.\n", *obu_type, *obu_size));
		gf_bs_seek(bs, pos + *obu_size);
		break;
	}
	return e;
}