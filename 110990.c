static u32 icount(u32 v)
{
	u32 ret = 0;
	while (v) {
		ret += v & 1;
		v >>= 1;
	}
	return(ret);
}