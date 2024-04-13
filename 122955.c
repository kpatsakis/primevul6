static void make_secret(uint8_t *digest, const uint8_t *vector,
			const char *secret, const uint8_t *value)
{
	MD5_CTX context;
        int             i;

	MD5Init(&context);
	MD5Update(&context, vector, AUTH_VECTOR_LEN);
	MD5Update(&context, secret, strlen(secret));
	MD5Final(digest, &context);

        for ( i = 0; i < AUTH_VECTOR_LEN; i++ ) {
		digest[i] ^= value[i];
        }
}