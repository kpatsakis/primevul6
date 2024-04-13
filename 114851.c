bool hex2bin(unsigned char *p, const char *hexstr, size_t len)
{
	bool ret = false;

	while (*hexstr && len) {
		char hex_byte[4];
		unsigned int v;

		if (unlikely(!hexstr[1])) {
			applog(LOG_ERR, "hex2bin str truncated");
			return ret;
		}

		memset(hex_byte, 0, 4);
		hex_byte[0] = hexstr[0];
		hex_byte[1] = hexstr[1];

		if (unlikely(sscanf(hex_byte, "%x", &v) != 1)) {
			applog(LOG_ERR, "hex2bin sscanf '%s' failed", hex_byte);
			return ret;
		}

		*p = (unsigned char) v;

		p++;
		hexstr += 2;
		len--;
	}

	if (likely(len == 0 && *hexstr == 0))
		ret = true;
	return ret;
}