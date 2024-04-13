u64 gf_m2ts_get_pts(unsigned char *data)
{
	u64 pts;
	u32 val;
	pts = (u64)((data[0] >> 1) & 0x07) << 30;
	val = (data[1] << 8) | data[2];
	pts |= (u64)(val >> 1) << 15;
	val = (data[3] << 8) | data[4];
	pts |= (u64)(val >> 1);
	return pts;
}