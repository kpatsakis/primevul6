ca_getreq(struct iked *env, struct imsg *imsg)
{
	struct ca_store		*store = env->sc_priv;
	struct iked_sahdr	 sh;
	uint8_t			 type, more;
	uint8_t			*ptr;
	size_t			 len;
	unsigned int		 i;
	X509			*ca = NULL, *cert = NULL;
	struct ibuf		*buf;
	struct iked_static_id	 id;
	char			 idstr[IKED_ID_SIZE];

	ptr = (uint8_t *)imsg->data;
	len = IMSG_DATA_SIZE(imsg);
	i = sizeof(id) + sizeof(type) + sizeof(sh) + sizeof(more);
	if (len < i)
		return (-1);

	memcpy(&id, ptr, sizeof(id));
	if (id.id_type == IKEV2_ID_NONE)
		return (-1);
	memcpy(&sh, ptr + sizeof(id), sizeof(sh));
	memcpy(&type, ptr + sizeof(id) + sizeof(sh), sizeof(type));
	memcpy(&more, ptr + sizeof(id) + sizeof(sh) + sizeof(type), sizeof(more));

	ptr += i;
	len -= i;

	switch (type) {
	case IKEV2_CERT_RSA_KEY:
	case IKEV2_CERT_ECDSA:
		/*
		 * Find a local raw public key that matches the type
		 * received in the CERTREQ payoad
		 */
		if (store->ca_pubkey.id_type != type ||
		    store->ca_pubkey.id_buf == NULL)
			goto fallback;

		buf = ibuf_dup(store->ca_pubkey.id_buf);
		log_debug("%s: using local public key of type %s", __func__,
		    print_map(type, ikev2_cert_map));
		break;
	case IKEV2_CERT_X509_CERT:
		if (len == 0 || len % SHA_DIGEST_LENGTH) {
			log_info("%s: invalid CERTREQ data.",
			    SPI_SH(&sh, __func__));
			return (-1);
		}

		/*
		 * Find a local certificate signed by any of the CAs
		 * received in the CERTREQ payload
		 */
		for (i = 0; i < len; i += SHA_DIGEST_LENGTH) {
			if ((ca = ca_by_subjectpubkey(store->ca_cas, ptr + i,
			    SHA_DIGEST_LENGTH)) == NULL)
				continue;

			log_debug("%s: found CA %s", __func__, ca->name);

			if ((cert = ca_by_issuer(store->ca_certs,
			    X509_get_subject_name(ca), &id)) != NULL) {
				/* XXX
				 * should we re-validate our own cert here?
				 */
				break;
			}
		}
		/* Fallthrough */
	case IKEV2_CERT_NONE:
 fallback:
		/*
		 * If no certificate or key matching any of the trust-anchors
		 * was found and this was the last CERTREQ, try to find one with
		 * subjectAltName matching the ID
		 */
		if (more)
			return (0);

		if (cert == NULL)
			cert = ca_by_subjectaltname(store->ca_certs, &id);

		/* If there is no matching certificate use local raw pubkey */
		if (cert == NULL) {
			if (ikev2_print_static_id(&id, idstr, sizeof(idstr)) == -1)
				return (-1);
			log_info("%s: no valid local certificate found for %s",
			    SPI_SH(&sh, __func__), idstr);
			ca_store_certs_info(SPI_SH(&sh, __func__),
			    store->ca_certs);
			if (store->ca_pubkey.id_buf == NULL)
				return (-1);
			buf = ibuf_dup(store->ca_pubkey.id_buf);
			type = store->ca_pubkey.id_type;
			log_info("%s: using local public key of type %s",
			    SPI_SH(&sh, __func__),
			    print_map(type, ikev2_cert_map));
			break;
		}

		log_debug("%s: found local certificate %s", __func__,
		    cert->name);

		if ((buf = ca_x509_serialize(cert)) == NULL)
			return (-1);
		break;
	default:
		log_warnx("%s: unknown cert type requested",
		    SPI_SH(&sh, __func__));
		return (-1);
	}

	ca_setcert(env, &sh, NULL, type,
	    ibuf_data(buf), ibuf_size(buf), PROC_IKEV2);
	ibuf_release(buf);

	return (0);
}