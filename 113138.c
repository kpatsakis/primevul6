ca_reload(struct iked *env)
{
	struct ca_store		*store = env->sc_priv;
	uint8_t			 md[EVP_MAX_MD_SIZE];
	char			 file[PATH_MAX];
	struct iovec		 iov[2];
	struct dirent		*entry;
	STACK_OF(X509_OBJECT)	*h;
	X509_OBJECT		*xo;
	X509			*x509;
	DIR			*dir;
	int			 i, len, iovcnt = 0;

	/*
	 * Load CAs
	 */
	if ((dir = opendir(IKED_CA_DIR)) == NULL)
		return (-1);

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type != DT_REG) &&
		    (entry->d_type != DT_LNK))
			continue;

		if (snprintf(file, sizeof(file), "%s%s",
		    IKED_CA_DIR, entry->d_name) < 0)
			continue;

		if (!X509_load_cert_file(store->ca_calookup, file,
		    X509_FILETYPE_PEM)) {
			log_warn("%s: failed to load ca file %s", __func__,
			    entry->d_name);
			ca_sslerror(__func__);
			continue;
		}
		log_debug("%s: loaded ca file %s", __func__, entry->d_name);
	}
	closedir(dir);

	/*
	 * Load CRLs for the CAs
	 */
	if ((dir = opendir(IKED_CRL_DIR)) == NULL)
		return (-1);

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type != DT_REG) &&
		    (entry->d_type != DT_LNK))
			continue;

		if (snprintf(file, sizeof(file), "%s%s",
		    IKED_CRL_DIR, entry->d_name) < 0)
			continue;

		if (!X509_load_crl_file(store->ca_calookup, file,
		    X509_FILETYPE_PEM)) {
			log_warn("%s: failed to load crl file %s", __func__,
			    entry->d_name);
			ca_sslerror(__func__);
			continue;
		}

		/* Only enable CRL checks if we actually loaded a CRL */
		X509_STORE_set_flags(store->ca_cas, X509_V_FLAG_CRL_CHECK);

		log_debug("%s: loaded crl file %s", __func__, entry->d_name);
	}
	closedir(dir);

	/*
	 * Save CAs signatures for the IKEv2 CERTREQ
	 */
	ibuf_release(env->sc_certreq);
	if ((env->sc_certreq = ibuf_new(NULL, 0)) == NULL)
		return (-1);

	h = store->ca_cas->objs;
	for (i = 0; i < sk_X509_OBJECT_num(h); i++) {
		xo = sk_X509_OBJECT_value(h, i);
		if (xo->type != X509_LU_X509)
			continue;

		x509 = xo->data.x509;
		len = sizeof(md);
		ca_subjectpubkey_digest(x509, md, &len);
		log_debug("%s: %s", __func__, x509->name);

		if (ibuf_add(env->sc_certreq, md, len) != 0) {
			ibuf_release(env->sc_certreq);
			env->sc_certreq = NULL;
			return (-1);
		}
	}

	if (ibuf_length(env->sc_certreq)) {
		env->sc_certreqtype = IKEV2_CERT_X509_CERT;
		iov[0].iov_base = &env->sc_certreqtype;
		iov[0].iov_len = sizeof(env->sc_certreqtype);
		iovcnt++;
		iov[1].iov_base = ibuf_data(env->sc_certreq);
		iov[1].iov_len = ibuf_length(env->sc_certreq);
		iovcnt++;

		log_debug("%s: loaded %zu ca certificate%s", __func__,
		    ibuf_length(env->sc_certreq) / SHA_DIGEST_LENGTH,
		    ibuf_length(env->sc_certreq) == SHA_DIGEST_LENGTH ?
		    "" : "s");

		(void)proc_composev(&env->sc_ps, PROC_IKEV2, IMSG_CERTREQ,
		    iov, iovcnt);
	}

	/*
	 * Load certificates
	 */
	if ((dir = opendir(IKED_CERT_DIR)) == NULL)
		return (-1);

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type != DT_REG) &&
		    (entry->d_type != DT_LNK))
			continue;

		if (snprintf(file, sizeof(file), "%s%s",
		    IKED_CERT_DIR, entry->d_name) < 0)
			continue;

		if (!X509_load_cert_file(store->ca_certlookup, file,
		    X509_FILETYPE_PEM)) {
			log_warn("%s: failed to load cert file %s", __func__,
			    entry->d_name);
			ca_sslerror(__func__);
			continue;
		}
		log_debug("%s: loaded cert file %s", __func__, entry->d_name);
	}
	closedir(dir);

	h = store->ca_certs->objs;
	for (i = 0; i < sk_X509_OBJECT_num(h); i++) {
		xo = sk_X509_OBJECT_value(h, i);
		if (xo->type != X509_LU_X509)
			continue;

		x509 = xo->data.x509;

		(void)ca_validate_cert(env, NULL, x509, 0);
	}

	if (!env->sc_certreqtype)
		env->sc_certreqtype = store->ca_pubkey.id_type;

	log_debug("%s: local cert type %s", __func__,
	    print_map(env->sc_certreqtype, ikev2_cert_map));

	iov[0].iov_base = &env->sc_certreqtype;
	iov[0].iov_len = sizeof(env->sc_certreqtype);
	if (iovcnt == 0)
		iovcnt++;
	(void)proc_composev(&env->sc_ps, PROC_IKEV2, IMSG_CERTREQ, iov, iovcnt);

	return (0);
}