u32 gf_av1_leb128_size(u64 value)
{
	u32 gf_av1_leb128_size = 0;
	do {
		++gf_av1_leb128_size;
	} while ((value >>= 7) != 0);

	return gf_av1_leb128_size;
}