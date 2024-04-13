static void vp9_tile_info(GF_BitStream *bs, int Sb64Cols)
{
	Bool tile_rows_log2;
	int minLog2TileCols = calc_min_log2_tile_cols(Sb64Cols);
	int maxLog2TileCols = calc_max_log2_tile_cols(Sb64Cols);
	int tile_cols_log2 = minLog2TileCols;
	while (tile_cols_log2 < maxLog2TileCols) {
		Bool increment_tile_cols_log2 = gf_bs_read_int_log(bs, 1, "increment_tile_cols_log2");
		if (increment_tile_cols_log2)
			tile_cols_log2++;
		else
			break;
	}
	tile_rows_log2 = gf_bs_read_int_log(bs, 1, "tile_rows_log2");
	if (tile_rows_log2) {
		/*Bool increment_tile_rows_log2 = */gf_bs_read_int_log(bs, 1, "increment_tile_rows_log2");
		//tile_rows_log2 += increment_tile_rows_log2;
	}
}