load_public_key (MonoArray *pkey, MonoDynamicImage *assembly) {
	gsize len;
	guint32 token = 0;
	char blob_size [6];
	char *b = blob_size;

	if (!pkey)
		return token;

	len = mono_array_length (pkey);
	mono_metadata_encode_value (len, b, &b);
	token = mono_image_add_stream_data (&assembly->blob, blob_size, b - blob_size);
	mono_image_add_stream_data (&assembly->blob, mono_array_addr (pkey, char, 0), len);

	assembly->public_key = g_malloc (len);
	memcpy (assembly->public_key, mono_array_addr (pkey, char, 0), len);
	assembly->public_key_len = len;

	/* Special case: check for ECMA key (16 bytes) */
	if ((len == MONO_ECMA_KEY_LENGTH) && mono_is_ecma_key (mono_array_addr (pkey, char, 0), len)) {
		/* In this case we must reserve 128 bytes (1024 bits) for the signature */
		assembly->strong_name_size = MONO_DEFAULT_PUBLIC_KEY_LENGTH;
	} else if (len >= MONO_PUBLIC_KEY_HEADER_LENGTH + MONO_MINIMUM_PUBLIC_KEY_LENGTH) {
		/* minimum key size (in 2.0) is 384 bits */
		assembly->strong_name_size = len - MONO_PUBLIC_KEY_HEADER_LENGTH;
	} else {
		/* FIXME - verifier */
		g_warning ("Invalid public key length: %d bits (total: %d)", (int)MONO_PUBLIC_KEY_BIT_SIZE (len), (int)len);
		assembly->strong_name_size = MONO_DEFAULT_PUBLIC_KEY_LENGTH; /* to be safe */
	}
	assembly->strong_name = g_malloc0 (assembly->strong_name_size);

	return token;
}