static void rewrite_pts_dts(unsigned char *ptr, u64 TS)
{
	ptr[0] &= 0xf1;
	ptr[0] |= (unsigned char)((TS&0x1c0000000ULL)>>29);
	ptr[1]  = (unsigned char)((TS&0x03fc00000ULL)>>22);
	ptr[2] &= 0x1;
	ptr[2] |= (unsigned char)((TS&0x0003f8000ULL)>>14);
	ptr[3]  = (unsigned char)((TS&0x000007f80ULL)>>7);
	ptr[4] &= 0x1;
	ptr[4] |= (unsigned char)((TS&0x00000007fULL)<<1);

	assert(((u64)(ptr[0]&0xe)<<29) + ((u64)ptr[1]<<22) + ((u64)(ptr[2]&0xfe)<<14) + ((u64)ptr[3]<<7) + ((ptr[4]&0xfe)>>1) == TS);
}