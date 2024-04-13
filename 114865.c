bool hash_target_check(const unsigned char *hash, const unsigned char *target)
{
	const uint32_t *h32 = (uint32_t*)&hash[0];
	const uint32_t *t32 = (uint32_t*)&target[0];
	for (int i = 7; i >= 0; --i) {
		uint32_t h32i = le32toh(h32[i]);
		uint32_t t32i = le32toh(t32[i]);
		if (h32i > t32i)
			return false;
		if (h32i < t32i)
			return true;
	}
	return true;
}