be4(const unsigned char *p)
{
	return ((((int64_t)p[0]) << 24) + (p[1] << 16) + (p[2] << 8) + (p[3]));
}