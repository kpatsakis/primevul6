sc_oberthur_get_certificate_authority(struct sc_pkcs15_der *der, int *out_authority)
{
#ifdef ENABLE_OPENSSL
	X509	*x;
	BUF_MEM buf_mem;
	BIO *bio = NULL;
	BASIC_CONSTRAINTS *bs = NULL;

	if (!der)
		return SC_ERROR_INVALID_ARGUMENTS;

	buf_mem.data = malloc(der->len);
	if (!buf_mem.data)
		return SC_ERROR_OUT_OF_MEMORY;

	memcpy(buf_mem.data, der->value, der->len);
	buf_mem.max = buf_mem.length = der->len;

	bio = BIO_new(BIO_s_mem());
	if (!bio) {
		free(buf_mem.data);
		return SC_ERROR_OUT_OF_MEMORY;
	}

	BIO_set_mem_buf(bio, &buf_mem, BIO_NOCLOSE);
	x = d2i_X509_bio(bio, 0);
	free(buf_mem.data);
	BIO_free(bio);
	if (!x)
		return SC_ERROR_INVALID_DATA;

	bs = (BASIC_CONSTRAINTS *)X509_get_ext_d2i(x, NID_basic_constraints, NULL, NULL);
	if (out_authority)
		*out_authority = (bs && bs->ca);

	X509_free(x);

	return SC_SUCCESS;
#else
	return SC_ERROR_NOT_SUPPORTED;
#endif
}