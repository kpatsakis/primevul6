bool fulltest(const unsigned char *hash, const unsigned char *target)
{
	uint32_t *hash32 = (uint32_t *)hash;
	uint32_t *target32 = (uint32_t *)target;
	bool rc = true;
	int i;

	for (i = 28 / 4; i >= 0; i--) {
		uint32_t h32tmp = le32toh(hash32[i]);
		uint32_t t32tmp = le32toh(target32[i]);

		if (h32tmp > t32tmp) {
			rc = false;
			break;
		}
		if (h32tmp < t32tmp) {
			rc = true;
			break;
		}
	}

	if (opt_debug) {
		unsigned char hash_swap[32], target_swap[32];
		char *hash_str, *target_str;

		swab256(hash_swap, hash);
		swab256(target_swap, target);
		hash_str = bin2hex(hash_swap, 32);
		target_str = bin2hex(target_swap, 32);

		applog(LOG_DEBUG, " Proof: %s\nTarget: %s\nTrgVal? %s",
			hash_str,
			target_str,
			rc ? "YES (hash <= target)" :
			     "no (false positive; hash > target)");

		free(hash_str);
		free(target_str);
	}

	return rc;
}