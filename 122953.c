int rad_tunnel_pwdecode(uint8_t *passwd, int *pwlen, const char *secret,
			const char *vector)
{
	uint8_t		buffer[AUTH_VECTOR_LEN + MAX_STRING_LEN + 3];
	uint8_t		digest[AUTH_VECTOR_LEN];
	uint8_t		decrypted[MAX_STRING_LEN + 1];
	int		secretlen;
	unsigned	i, n, len;

	len = *pwlen;

	/*
	 *	We need at least a salt.
	 */
	if (len < 2) {
		librad_log("tunnel password is too short");
		return -1;
	}

	/*
	 *	There's a salt, but no password.  Or, there's a salt
	 *	and a 'data_len' octet.  It's wrong, but at least we
	 *	can figure out what it means: the password is empty.
	 *
	 *	Note that this means we ignore the 'data_len' field,
	 *	if the attribute length tells us that there's no
	 *	more data.  So the 'data_len' field may be wrong,
	 *	but that's ok...
	 */
	if (len <= 3) {
		passwd[0] = 0;
		*pwlen = 0;
		return 0;
	}

	len -= 2;		/* discount the salt */

	/*
	 *	Use the secret to setup the decryption digest
	 */
	secretlen = strlen(secret);

	/*
	 *	Set up the initial key:
	 *
	 *	 b(1) = MD5(secret + vector + salt)
	 */
	memcpy(buffer, secret, secretlen);
	memcpy(buffer + secretlen, vector, AUTH_VECTOR_LEN);
	memcpy(buffer + secretlen + AUTH_VECTOR_LEN, passwd, 2);
	librad_md5_calc(digest, buffer, secretlen + AUTH_VECTOR_LEN + 2);

	/*
	 *	A quick check: decrypt the first octet of the password,
	 *	which is the 'data_len' field.  Ensure it's sane.
	 *
	 *	'n' doesn't include the 'data_len' octet
	 *	'len' does.
	 */
	n = passwd[2] ^ digest[0];
	if (n >= len) {
		librad_log("tunnel password is too long for the attribute");
		return -1;
	}

	/*
	 *	Loop over the data, decrypting it, and generating
	 *	the key for the next round of decryption.
	 */
	for (n = 0; n < len; n += AUTH_PASS_LEN) {
		for (i = 0; i < AUTH_PASS_LEN; i++) {
			decrypted[n + i] = passwd[n + i + 2] ^ digest[i];

			/*
			 *	Encrypted password may not be aligned
			 *	on 16 octets, so we catch that here...
			 */
			if ((n + i) == len) break;
		}

		/*
		 *	Update the digest, based on
		 *
		 *	b(n) = MD5(secret + cleartext(n-1)
		 *
		 *	but only if there's more data...
		 */
		memcpy(buffer + secretlen, passwd + n + 2, AUTH_PASS_LEN);
		librad_md5_calc(digest, buffer, secretlen + AUTH_PASS_LEN);
	}

	/*
	 *	We've already validated the length of the decrypted
	 *	password.  Copy it back to the caller.
	 */
	memcpy(passwd, decrypted + 1, decrypted[0]);
	passwd[decrypted[0]] = 0;
	*pwlen = decrypted[0];

	return decrypted[0];
}