put16(unsigned char *cp, unsigned short x)
{
	*cp++ = x >> 8;
	*cp++ = x;

	return cp;
}