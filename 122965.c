int rad_tunnel_pwencode(char *passwd, int *pwlen, const char *secret,
			const char *vector)
{
	uint8_t	buffer[AUTH_VECTOR_LEN + MAX_STRING_LEN + 3];
	unsigned char	digest[AUTH_VECTOR_LEN];
	char*   salt;
	int	i, n, secretlen;
	unsigned len, n2;

	len = *pwlen;

	if (len > 127) len = 127;
	/*
	 * Shift the password 3 positions right to place a salt and original
	 * length, tag will be added automatically on packet send
	 */
	for (n=len ; n>=0 ; n--) passwd[n+3] = passwd[n];
	salt = passwd;
	passwd += 2;
	/*
	 * save original password length as first password character;
	 */
	*passwd = len;
	len += 1;


	/*
	 *	Generate salt.  The RFC's say:
	 *
	 *	The high bit of salt[0] must be set, each salt in a
	 *	packet should be unique, and they should be random
	 *
	 *	So, we set the high bit, add in a counter, and then
	 *	add in some CSPRNG data.  should be OK..
	 */
	salt[0] = (0x80 | ( ((salt_offset++) & 0x0f) << 3) |
		   (lrad_rand() & 0x07));
	salt[1] = lrad_rand();

	/*
	 *	Padd password to multiple of AUTH_PASS_LEN bytes.
	 */
	n = len % AUTH_PASS_LEN;
	if (n) {
		n = AUTH_PASS_LEN - n;
		for (; n > 0; n--, len++)
			passwd[len] = 0;
	}
	/* set new password length */
	*pwlen = len + 2;

	/*
	 *	Use the secret to setup the decryption digest
	 */
	secretlen = strlen(secret);
	memcpy(buffer, secret, secretlen);

	for (n2 = 0; n2 < len; n2+=AUTH_PASS_LEN) {
		if (!n2) {
			memcpy(buffer + secretlen, vector, AUTH_VECTOR_LEN);
			memcpy(buffer + secretlen + AUTH_VECTOR_LEN, salt, 2);
			librad_md5_calc(digest, buffer, secretlen + AUTH_VECTOR_LEN + 2);
		} else {
			memcpy(buffer + secretlen, passwd + n2 - AUTH_PASS_LEN, AUTH_PASS_LEN);
			librad_md5_calc(digest, buffer, secretlen + AUTH_PASS_LEN);
		}

		for (i = 0; i < AUTH_PASS_LEN; i++) {
			passwd[i + n2] ^= digest[i];
		}
	}
	passwd[n2] = 0;
	return 0;
}