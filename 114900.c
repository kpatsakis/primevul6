bool hash_target_check_v(const unsigned char *hash, const unsigned char *target)
{
	bool rc;

	rc = hash_target_check(hash, target);

	if (opt_debug) {
		unsigned char hash_swap[32], target_swap[32];
		char *hash_str, *target_str;

		for (int i = 0; i < 32; ++i) {
			hash_swap[i] = hash[31-i];
			target_swap[i] = target[31-i];
		}

		hash_str = bin2hex(hash_swap, 32);
		target_str = bin2hex(target_swap, 32);

		applog(LOG_DEBUG, " Proof: %s\nTarget: %s\nTrgVal? %s",
			hash_str,
			target_str,
			rc ? "YES (hash < target)" :
			     "no (false positive; hash > target)");

		free(hash_str);
		free(target_str);
	}

	return rc;
}