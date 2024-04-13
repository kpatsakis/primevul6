ca_setcert(struct iked *env, struct iked_sahdr *sh, struct iked_id *id,
    uint8_t type, uint8_t *data, size_t len, enum privsep_procid procid)
{
	struct iovec		iov[4];
	int			iovcnt = 0;
	struct iked_static_id	idb;

	/* Must send the cert and a valid Id to the ca process */
	if (procid == PROC_CERT) {
		if (id == NULL || id->id_type == IKEV2_ID_NONE ||
		    ibuf_length(id->id_buf) > IKED_ID_SIZE)
			return (-1);
		bzero(&idb, sizeof(idb));

		/* Convert to a static Id */
		idb.id_type = id->id_type;
		idb.id_offset = id->id_offset;
		idb.id_length = ibuf_length(id->id_buf);
		memcpy(&idb.id_data, ibuf_data(id->id_buf),
		    ibuf_length(id->id_buf));

		iov[iovcnt].iov_base = &idb;
		iov[iovcnt].iov_len = sizeof(idb);
		iovcnt++;
	}

	iov[iovcnt].iov_base = sh;
	iov[iovcnt].iov_len = sizeof(*sh);
	iovcnt++;
	iov[iovcnt].iov_base = &type;
	iov[iovcnt].iov_len = sizeof(type);
	iovcnt++;
	if (data != NULL) {
		iov[iovcnt].iov_base = data;
		iov[iovcnt].iov_len = len;
		iovcnt++;
	}

	if (proc_composev(&env->sc_ps, procid, IMSG_CERT, iov, iovcnt) == -1)
		return (-1);
	return (0);
}