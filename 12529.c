_cdf_tole2(uint16_t sv)
{
	uint16_t rv;
	uint8_t *s = (uint8_t *)(void *)&sv;
	uint8_t *d = (uint8_t *)(void *)&rv;
	d[0] = s[1];
	d[1] = s[0];
	return rv;
}