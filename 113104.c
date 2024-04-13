ca_privkey_to_method(struct iked_id *privkey)
{
	BIO		*rawcert = NULL;
	EC_KEY		*ec = NULL;
	const EC_GROUP	*group = NULL;
	uint8_t	 method = IKEV2_AUTH_NONE;

	switch (privkey->id_type) {
	case IKEV2_CERT_RSA_KEY:
		method = IKEV2_AUTH_RSA_SIG;
		break;
	case IKEV2_CERT_ECDSA:
		if ((rawcert = BIO_new_mem_buf(ibuf_data(privkey->id_buf),
		    ibuf_length(privkey->id_buf))) == NULL)
			goto out;
		if ((ec = d2i_ECPrivateKey_bio(rawcert, NULL)) == NULL)
			goto out;
		if ((group = EC_KEY_get0_group(ec)) == NULL)
			goto out;
		switch (EC_GROUP_get_degree(group)) {
		case 256:
			method = IKEV2_AUTH_ECDSA_256;
			break;
		case 384:
			method = IKEV2_AUTH_ECDSA_384;
			break;
		case 521:
			method = IKEV2_AUTH_ECDSA_521;
			break;
		}
	}

	log_debug("%s: type %s method %s", __func__,
	    print_map(privkey->id_type, ikev2_cert_map),
	    print_map(method, ikev2_auth_map));

 out:
	if (ec != NULL)
		EC_KEY_free(ec);
	if (rawcert != NULL)
		BIO_free(rawcert);

	return (method);
}