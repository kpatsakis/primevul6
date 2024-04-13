static int calc_min_log2_tile_cols(int Sb64Cols) {
	int minLog2 = 0;
	while ((VP9_MAX_TILE_WIDTH_B64 << minLog2) < Sb64Cols)
		minLog2++;

	return minLog2;
}