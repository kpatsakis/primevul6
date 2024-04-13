void hash_data(unsigned char *out_hash, const unsigned char *data)
{
	unsigned char blkheader[80];
	
	// data is past the first SHA256 step (padding and interpreting as big endian on a little endian platform), so we need to flip each 32-bit chunk around to get the original input block header
	swap32yes(blkheader, data, 80 / 4);
	
	// double-SHA256 to get the block hash
	gen_hash(blkheader, out_hash, 80);
}