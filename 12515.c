cdf_getuint32(const uint8_t *p, size_t offs)
{
	uint32_t rv;
	(void)memcpy(&rv, p + offs * sizeof(uint32_t), sizeof(rv));
	return CDF_TOLE4(rv);
}