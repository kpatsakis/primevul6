static void make_passwd(uint8_t *output, int *outlen,
			const uint8_t *input, int inlen,
			const char *secret, const uint8_t *vector)
{
	MD5_CTX context, old;
	uint8_t	digest[AUTH_VECTOR_LEN];
	uint8_t passwd[MAX_PASS_LEN];
	int	i, n;
	int	len;

	/*
	 *	If the length is zero, round it up.
	 */
	len = inlen;
	if (len == 0) {
		len = AUTH_PASS_LEN;
	}
	else if (len > MAX_PASS_LEN) len = MAX_PASS_LEN;

	else if ((len & 0x0f) != 0) {
		len += 0x0f;
		len &= ~0x0f;
	}
	*outlen = len;

	memcpy(passwd, input, len);
	memset(passwd + len, 0, sizeof(passwd) - len);

	MD5Init(&context);
	MD5Update(&context, secret, strlen(secret));
	old = context;

	/*
	 *	Do first pass.
	 */
	MD5Update(&context, vector, AUTH_PASS_LEN);

	for (n = 0; n < len; n += AUTH_PASS_LEN) {
		if (n > 0) {
			context = old;
			MD5Update(&context,
				       passwd + n - AUTH_PASS_LEN,
				       AUTH_PASS_LEN);
		}

		MD5Final(digest, &context);
		for (i = 0; i < AUTH_PASS_LEN; i++) {
			passwd[i + n] ^= digest[i];
		}
	}

	memcpy(output, passwd, len);
}