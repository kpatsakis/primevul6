static int calc_max_log2_tile_cols(int Sb64Cols) {
	int maxLog2 = 1;
	while ((Sb64Cols >> maxLog2) >= VP9_MIN_TILE_WIDTH_B64)
		maxLog2++;

	return maxLog2 - 1;
}