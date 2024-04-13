int rad_pwdecode(char *passwd, int pwlen, const char *secret,
		 const char *vector)
{
	uint8_t	buffer[AUTH_VECTOR_LEN + MAX_STRING_LEN + 1];
	char	digest[AUTH_VECTOR_LEN];
	char	r[AUTH_VECTOR_LEN];
	char	*s;
	int	i, n, secretlen;
	int	rlen;

	/*
	 *	Use the secret to setup the decryption digest
	 */
	secretlen = strlen(secret);
	memcpy(buffer, secret, secretlen);
	memcpy(buffer + secretlen, vector, AUTH_VECTOR_LEN);
	librad_md5_calc((u_char *)digest, buffer, secretlen + AUTH_VECTOR_LEN);

	/*
	 *	Now we can decode the password *in place*
	 */
	memcpy(r, passwd, AUTH_PASS_LEN);
	for (i = 0; i < AUTH_PASS_LEN && i < pwlen; i++)
		passwd[i] ^= digest[i];

	if (pwlen <= AUTH_PASS_LEN) {
		passwd[pwlen+1] = 0;
		return pwlen;
	}

	/*
	 *	Length > AUTH_PASS_LEN, so we need to use the extended
	 *	algorithm.
	 */
	rlen = ((pwlen - 1) / AUTH_PASS_LEN) * AUTH_PASS_LEN;

	for (n = rlen; n > 0; n -= AUTH_PASS_LEN ) {
		s = (n == AUTH_PASS_LEN) ? r : (passwd + n - AUTH_PASS_LEN);
		memcpy(buffer + secretlen, s, AUTH_PASS_LEN);
		librad_md5_calc((u_char *)digest, buffer, secretlen + AUTH_PASS_LEN);
		for (i = 0; i < AUTH_PASS_LEN && (i + n) < pwlen; i++)
			passwd[i + n] ^= digest[i];
	}
	passwd[pwlen] = 0;

	return pwlen;
}