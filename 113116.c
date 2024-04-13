ca_getcert(struct iked *env, struct imsg *imsg)
{
	struct iked_sahdr	 sh;
	uint8_t			 type;
	uint8_t			*ptr;
	size_t			 len;
	struct iked_static_id	 id;
	unsigned int		 i;
	struct iovec		 iov[3];
	int			 iovcnt = 3, cmd, ret = 0;
	struct iked_id		 key;

	ptr = (uint8_t *)imsg->data;
	len = IMSG_DATA_SIZE(imsg);
	i = sizeof(id) + sizeof(sh) + sizeof(type);
	if (len < i)
		return (-1);

	memcpy(&id, ptr, sizeof(id));
	if (id.id_type == IKEV2_ID_NONE)
		return (-1);
	memcpy(&sh, ptr + sizeof(id), sizeof(sh));
	memcpy(&type, ptr + sizeof(id) + sizeof(sh), sizeof(uint8_t));

	ptr += i;
	len -= i;

	bzero(&key, sizeof(key));

	switch (type) {
	case IKEV2_CERT_X509_CERT:
		ret = ca_validate_cert(env, &id, ptr, len);
		if (ret == 0 && env->sc_ocsp_url) {
			ret = ocsp_validate_cert(env, &id, ptr, len, sh, type);
			if (ret == 0)
				return (0);
		}
		break;
	case IKEV2_CERT_RSA_KEY:
	case IKEV2_CERT_ECDSA:
		ret = ca_validate_pubkey(env, &id, ptr, len, NULL);
		break;
	case IKEV2_CERT_NONE:
		/* Fallback to public key */
		ret = ca_validate_pubkey(env, &id, NULL, 0, &key);
		if (ret == 0) {
			ptr = ibuf_data(key.id_buf);
			len = ibuf_length(key.id_buf);
			type = key.id_type;
		}
		break;
	default:
		log_debug("%s: unsupported cert type %d", __func__, type);
		ret = -1;
		break;
	}

	if (ret == 0)
		cmd = IMSG_CERTVALID;
	else
		cmd = IMSG_CERTINVALID;

	iov[0].iov_base = &sh;
	iov[0].iov_len = sizeof(sh);
	iov[1].iov_base = &type;
	iov[1].iov_len = sizeof(type);
	iov[2].iov_base = ptr;
	iov[2].iov_len = len;

	if (proc_composev(&env->sc_ps, PROC_IKEV2, cmd, iov, iovcnt) == -1)
		return (-1);
	return (0);
}