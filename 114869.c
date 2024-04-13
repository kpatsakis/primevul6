bool fulltest(const unsigned char *hash, const unsigned char *target)
{
	unsigned char hash2[32];
	swap32tobe(hash2, hash, 32 / 4);
	return hash_target_check_v(hash2, target);
}