le4(const unsigned char *p)
{
	return ((p[0] << 16) + (((int64_t)p[1]) << 24) + (p[2] << 0) + (p[3] << 8));
}