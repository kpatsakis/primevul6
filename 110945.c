static void av1_parse_tile_info(GF_BitStream *bs, AV1State *state)
{
	u32 i;
	u32 MiCols = 2 * ((state->width + 7) >> 3);
	u32 MiRows = 2 * ((state->height + 7) >> 3);
	u32 sbCols = state->use_128x128_superblock ? ((MiCols + 31) >> 5) : ((MiCols + 15) >> 4);
	u32 sbRows = state->use_128x128_superblock ? ((MiRows + 31) >> 5) : ((MiRows + 15) >> 4);
	u32 sbShift = state->use_128x128_superblock ? 5 : 4;
	u32 sbSize = sbShift + 2;
	u32 maxTileWidthSb = MAX_TILE_WIDTH >> sbSize;
	u32 maxTileAreaSb = MAX_TILE_AREA >> (2 * sbSize);
	u32 minLog2tileCols = aom_av1_tile_log2(maxTileWidthSb, sbCols);
	u32 maxLog2tileCols = aom_av1_tile_log2(1, MIN(sbCols, AV1_MAX_TILE_COLS));
	u32 maxLog2tileRows = aom_av1_tile_log2(1, MIN(sbRows, AV1_MAX_TILE_ROWS));
	u32 minLog2Tiles = MAX(minLog2tileCols, aom_av1_tile_log2(maxTileAreaSb, sbRows * sbCols));
	Bool uniform_tile_spacing_flag = gf_bs_read_int_log(bs, 1, "uniform_tile_spacing_flag");
	if (uniform_tile_spacing_flag) {
		u32 startSb, tileWidthSb, tileHeightSb, minLog2tileRows;
		state->tileColsLog2 = minLog2tileCols;
		while (state->tileColsLog2 < maxLog2tileCols) {
			Bool increment_tile_cols_log2 = gf_bs_read_int_log(bs, 1, "increment_tile_cols_log2");
			if (increment_tile_cols_log2 == 1)
				state->tileColsLog2++;
			else
				break;
		}

		tileWidthSb = (sbCols + (1 << state->tileColsLog2) - 1) >> state->tileColsLog2;
		i = 0;
		for (startSb = 0; startSb < sbCols; startSb += tileWidthSb) {
			i += 1;
		}
		state->tileCols = i;
		minLog2tileRows = MAX((int)(minLog2Tiles - state->tileColsLog2), 0);
		state->tileRowsLog2 = minLog2tileRows;
		while (state->tileRowsLog2 < maxLog2tileRows) {
			Bool increment_tile_rows_log2 = gf_bs_read_int_log(bs, 1, "increment_tile_rows_log2");
			if (increment_tile_rows_log2 == 1)
				state->tileRowsLog2++;
			else
				break;
		}

		tileHeightSb = (sbRows + (1 << state->tileRowsLog2) - 1) >> state->tileRowsLog2;
		i = 0;
		for (startSb = 0; startSb < sbRows; startSb += tileHeightSb) {
			i += 1;
		}
		state->tileRows = i;
	}
	else {
		u32 startSb, maxTileHeightSb, widestTileSb;
		widestTileSb = 0;
		startSb = 0;
		for (i = 0; startSb < sbCols; i++) {
			u32 maxWidth = MIN((int)(sbCols - startSb), maxTileWidthSb);
			u32 width_in_sbs_minus_1 = av1_read_ns(bs, maxWidth, "width_in_sbs_minus_1");
			u32 sizeSb = width_in_sbs_minus_1 + 1;
			widestTileSb = MAX(sizeSb, widestTileSb);
			startSb += sizeSb;
		}
		if (!widestTileSb) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[AV1] widest tile is 0, broken bitstream\n"));
			return;
		}
		state->tileCols = i;
		state->tileColsLog2 = aom_av1_tile_log2(1, state->tileCols);

		if (minLog2Tiles > 0)
			maxTileAreaSb = (sbRows * sbCols) >> (minLog2Tiles + 1);
		else
			maxTileAreaSb = sbRows * sbCols;
		maxTileHeightSb = MAX(maxTileAreaSb / widestTileSb, 1);

		startSb = 0;
		for (i = 0; startSb < sbRows; i++) {
			u32 maxHeight = MIN((int)(sbRows - startSb), maxTileHeightSb);
			u32 height_in_sbs_minus_1 = av1_read_ns(bs, maxHeight, "height_in_sbs_minus_1");
			u32 sizeSb = height_in_sbs_minus_1 + 1;
			startSb += sizeSb;
		}

		state->tileRows = i;
		state->tileRowsLog2 = aom_av1_tile_log2(1, state->tileRows);
	}
	if (state->tileColsLog2 > 0 || state->tileRowsLog2 > 0) {
		gf_bs_read_int_log(bs, state->tileRowsLog2 + state->tileColsLog2, "context_update_tile_id");
		state->tile_size_bytes = gf_bs_read_int_log(bs, 2, "tile_size_bytes_minus1") + 1;
	}
}