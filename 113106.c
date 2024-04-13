ca_x509_serialize(X509 *x509)
{
	long		 len;
	struct ibuf	*buf;
	uint8_t		*d = NULL;
	BIO		*out;

	if ((out = BIO_new(BIO_s_mem())) == NULL)
		return (NULL);
	if (!i2d_X509_bio(out, x509)) {
		BIO_free(out);
		return (NULL);
	}

	len = BIO_get_mem_data(out, &d);
	buf = ibuf_new(d, len);
	BIO_free(out);

	return (buf);
}