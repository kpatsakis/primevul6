static u32 aom_av1_tile_log2(u32 blkSize, u32 target)
{
	u32 k;
	for (k = 0; (blkSize << k) < target; k++) {
	}
	return k;
}