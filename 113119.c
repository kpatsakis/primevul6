ca_store_certs_info(const char *msg, X509_STORE *ctx)
{
	STACK_OF(X509_OBJECT)	*h;
	X509_OBJECT		*xo;
	X509			*cert;
	int			 i;

	h = ctx->objs;
	for (i = 0; i < sk_X509_OBJECT_num(h); i++) {
		xo = sk_X509_OBJECT_value(h, i);
		if (xo->type != X509_LU_X509)
			continue;
		cert = xo->data.x509;
		ca_cert_info(msg, cert);
	}
}