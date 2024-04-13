hash(const void *s)
{
	register uint32_t h;

	BAFHInit(h);
	BAFHUpdateStr_reg(h, s);
	BAFHFinish_reg(h);
	return (h);
}