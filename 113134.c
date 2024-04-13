ca_by_subjectpubkey(X509_STORE *ctx, uint8_t *sig, size_t siglen)
{
	STACK_OF(X509_OBJECT)	*h;
	X509_OBJECT		*xo;
	X509			*ca;
	int			 i;
	unsigned int		 len;
	uint8_t			 md[EVP_MAX_MD_SIZE];

	h = ctx->objs;

	for (i = 0; i < sk_X509_OBJECT_num(h); i++) {
		xo = sk_X509_OBJECT_value(h, i);
		if (xo->type != X509_LU_X509)
			continue;

		ca = xo->data.x509;
		len = sizeof(md);
		ca_subjectpubkey_digest(ca, md, &len);

		if (len == siglen && memcmp(md, sig, len) == 0)
			return (ca);
	}

	return (NULL);
}