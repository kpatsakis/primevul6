ca_by_issuer(X509_STORE *ctx, X509_NAME *subject, struct iked_static_id *id)
{
	STACK_OF(X509_OBJECT)	*h;
	X509_OBJECT		*xo;
	X509			*cert;
	int			 i;
	X509_NAME		*issuer;

	if (subject == NULL)
		return (NULL);

	h = ctx->objs;
	for (i = 0; i < sk_X509_OBJECT_num(h); i++) {
		xo = sk_X509_OBJECT_value(h, i);
		if (xo->type != X509_LU_X509)
			continue;

		cert = xo->data.x509;
		if ((issuer = X509_get_issuer_name(cert)) == NULL)
			continue;
		else if (X509_NAME_cmp(subject, issuer) == 0) {
			switch (id->id_type) {
			case IKEV2_ID_ASN1_DN:
				if (ca_x509_subject_cmp(cert, id) == 0)
					return (cert);
				break;
			default:
				if (ca_x509_subjectaltname_cmp(cert, id) == 0)
					return (cert);
				break;
			}
		}
	}

	return (NULL);
}