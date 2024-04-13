ca_privkey_serialize(EVP_PKEY *key, struct iked_id *id)
{
	RSA		*rsa = NULL;
	EC_KEY		*ec = NULL;
	uint8_t		*d;
	int		 len = 0;
	int		 ret = -1;

	switch (key->type) {
	case EVP_PKEY_RSA:
		id->id_type = 0;
		id->id_offset = 0;
		ibuf_release(id->id_buf);
		id->id_buf = NULL;

		if ((rsa = EVP_PKEY_get1_RSA(key)) == NULL)
			goto done;
		if ((len = i2d_RSAPrivateKey(rsa, NULL)) <= 0)
			goto done;
		if ((id->id_buf = ibuf_new(NULL, len)) == NULL)
			goto done;

		d = ibuf_data(id->id_buf);
		if (i2d_RSAPrivateKey(rsa, &d) != len) {
			ibuf_release(id->id_buf);
			id->id_buf = NULL;
			goto done;
		}

		id->id_type = IKEV2_CERT_RSA_KEY;
		break;
	case EVP_PKEY_EC:
		id->id_type = 0;
		id->id_offset = 0;
		ibuf_release(id->id_buf);
		id->id_buf = NULL;

		if ((ec = EVP_PKEY_get1_EC_KEY(key)) == NULL)
			goto done;
		if ((len = i2d_ECPrivateKey(ec, NULL)) <= 0)
			goto done;
		if ((id->id_buf = ibuf_new(NULL, len)) == NULL)
			goto done;

		d = ibuf_data(id->id_buf);
		if (i2d_ECPrivateKey(ec, &d) != len) {
			ibuf_release(id->id_buf);
			id->id_buf = NULL;
			goto done;
		}

		id->id_type = IKEV2_CERT_ECDSA;
		break;
	default:
		log_debug("%s: unsupported key type %d", __func__, key->type);
		return (-1);
	}

	log_debug("%s: type %s length %d", __func__,
	    print_map(id->id_type, ikev2_cert_map), len);

	ret = 0;
 done:
	if (rsa != NULL)
		RSA_free(rsa);
	if (ec != NULL)
		EC_KEY_free(ec);
	return (ret);
}