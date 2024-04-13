static GFINLINE s32 inverse_recenter(s32 r, u32 v)
{
	if ((s64)v > (s64)(2 * r))
		return v;
	else if (v & 1)
		return r - ((v + 1) >> 1);
	else
		return r + (v >> 1);
}