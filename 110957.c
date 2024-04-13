static GF_Err av1_parse_tile_group(GF_BitStream *bs, AV1State *state, u64 obu_start, u64 obu_size)
{
	u32 TileNum, tg_start = 0, tg_end = 0;
	Bool numTiles = state->tileCols * state->tileRows;
	Bool tile_start_and_end_present_flag = GF_FALSE;
	GF_Err e = GF_OK;
	if (numTiles > 1)
		tile_start_and_end_present_flag = gf_bs_read_int_log(bs, 1, "tile_start_and_end_present_flag");

	if (numTiles == 1 || !tile_start_and_end_present_flag) {
		tg_start = 0;
		tg_end = numTiles - 1;
		/*state->frame_state.tg[0].start_idx = 0;
		state->frame_state.tg[0].end_idx = numTiles - 1;*/
	}
	else {
		u32 tileBits = state->tileColsLog2 + state->tileRowsLog2;
		/*state->frame_state.tg[state->frame_state.tg_idx].start_idx*/ tg_start = gf_bs_read_int_log(bs, tileBits, "tg_start");
		/*state->frame_state.tg[state->frame_state.tg_idx].end_idx*/ tg_end = gf_bs_read_int_log(bs, tileBits, "tg_end");
	}
	/*state->frame_state.tg_idx++;*/

	gf_bs_align(bs);

	if (tg_end >= GF_ARRAY_LENGTH(state->frame_state.tiles))
		return GF_NON_COMPLIANT_BITSTREAM;

	state->frame_state.nb_tiles_in_obu = 0;
	for (TileNum = tg_start; TileNum <= tg_end; TileNum++) {
		u32 tile_start_offset, tile_size;
		/*u32 tileRow = TileNum / state->tileCols;
		u32 tileCol = TileNum % state->tileCols;*/
		Bool lastTile = TileNum == tg_end;
		u64 pos = gf_bs_get_position(bs);
		if (lastTile) {
			tile_start_offset = (u32)(pos - obu_start);
			tile_size = (u32)(obu_size - (pos - obu_start));
		}
		else {
			u64 tile_size_minus_1 = aom_av1_le(bs, state->tile_size_bytes, "tile_size_minus_1");
			pos = gf_bs_get_position(bs);
			tile_start_offset = (u32)(pos - obu_start);
			tile_size = (u32)(tile_size_minus_1 + 1/* + state->tile_size_bytes*/);
		}


		if (tile_start_offset + tile_size > obu_size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] Error parsing tile group, tile %d start %d + size %d exceeds OBU length %d\n", TileNum, tile_start_offset, tile_size, obu_size));
			e = GF_NON_COMPLIANT_BITSTREAM;
			break;
		}

		state->frame_state.tiles[state->frame_state.nb_tiles_in_obu].obu_start_offset = tile_start_offset;
		state->frame_state.tiles[state->frame_state.nb_tiles_in_obu].size = tile_size;
		gf_bs_skip_bytes(bs, tile_size);
		state->frame_state.nb_tiles_in_obu++;
	}
	if (tg_end == numTiles - 1) {
		av1_decode_frame_wrapup(state);
	}
	return e;
}