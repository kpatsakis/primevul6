static u32 ilog(u32 v, Bool dec)
{
	u32 ret = 0;
	if (dec && v) --v;
	while (v) {
		ret++;
		v >>= 1;
	}
	return (ret);
}