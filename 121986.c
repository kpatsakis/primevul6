R_API ut32 U(r_bin_java_swap_uint)(ut32 x) {
	const ut32 Byte0 = x & 0x000000FF;
	const ut32 Byte1 = x & 0x0000FF00;
	const ut32 Byte2 = x & 0x00FF0000;
	const ut32 Byte3 = x & 0xFF000000;
	return (Byte0 << 24) | (Byte1 << 8) | (Byte2 >> 8) | (Byte3 >> 24);
}