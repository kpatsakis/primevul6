ca_validate_cert(struct iked *env, struct iked_static_id *id,
    void *data, size_t len)
{
	struct ca_store	*store = env->sc_priv;
	X509_STORE_CTX	 csc;
	BIO		*rawcert = NULL;
	X509		*cert = NULL;
	EVP_PKEY	*pkey;
	int		 ret = -1, result, error;
	X509_NAME	*subject;
	const char	*errstr = "failed";

	if (len == 0) {
		/* Data is already an X509 certificate */
		cert = (X509 *)data;
	} else {
		/* Convert data to X509 certificate */
		if ((rawcert = BIO_new_mem_buf(data, len)) == NULL)
			goto done;
		if ((cert = d2i_X509_bio(rawcert, NULL)) == NULL)
			goto done;
	}

	/* Certificate needs a valid subjectName */
	if ((subject = X509_get_subject_name(cert)) == NULL) {
		errstr = "invalid subject";
		goto done;
	}

	if (id != NULL) {
		if ((pkey = X509_get_pubkey(cert)) == NULL) {
			errstr = "no public key in cert";
			goto done;
		}
		ret = ca_validate_pubkey(env, id, pkey, 0, NULL);
		EVP_PKEY_free(pkey);
		if (ret == 0) {
			errstr = "in public key file, ok";
			goto done;
		}

		switch (id->id_type) {
		case IKEV2_ID_ASN1_DN:
			if (ca_x509_subject_cmp(cert, id) < 0) {
				errstr = "ASN1_DN identifier mismatch";
				goto done;
			}
			break;
		default:
			if (ca_x509_subjectaltname_cmp(cert, id) != 0) {
				errstr = "invalid subjectAltName extension";
				goto done;
			}
			break;
		}
	}

	bzero(&csc, sizeof(csc));
	X509_STORE_CTX_init(&csc, store->ca_cas, cert, NULL);
	if (store->ca_cas->param->flags & X509_V_FLAG_CRL_CHECK) {
		X509_STORE_CTX_set_flags(&csc, X509_V_FLAG_CRL_CHECK);
		X509_STORE_CTX_set_flags(&csc, X509_V_FLAG_CRL_CHECK_ALL);
	}

	result = X509_verify_cert(&csc);
	error = csc.error;
	X509_STORE_CTX_cleanup(&csc);
	if (error != 0) {
		errstr = X509_verify_cert_error_string(error);
		goto done;
	}

	if (!result) {
		/* XXX should we accept self-signed certificates? */
		errstr = "rejecting self-signed certificate";
		goto done;
	}

	/* Success */
	ret = 0;
	errstr = "ok";

 done:
	if (cert != NULL)
		log_debug("%s: %s %.100s", __func__, cert->name, errstr);

	if (rawcert != NULL) {
		BIO_free(rawcert);
		if (cert != NULL)
			X509_free(cert);
	}

	return (ret);
}