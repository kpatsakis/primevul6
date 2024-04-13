ca_setreq(struct iked *env, struct iked_sa *sa,
    struct iked_static_id *localid, uint8_t type, uint8_t more, uint8_t *data,
    size_t len, enum privsep_procid procid)
{
	struct iovec		iov[5];
	int			iovcnt = 0;
	struct iked_static_id	idb;
	struct iked_id		id;
	int			ret = -1;

	/* Convert to a static Id */
	bzero(&id, sizeof(id));
	if (ikev2_policy2id(localid, &id, 1) != 0)
		return (-1);

	bzero(&idb, sizeof(idb));
	idb.id_type = id.id_type;
	idb.id_offset = id.id_offset;
	idb.id_length = ibuf_length(id.id_buf);
	memcpy(&idb.id_data, ibuf_data(id.id_buf),
	    ibuf_length(id.id_buf));
	iov[iovcnt].iov_base = &idb;
	iov[iovcnt].iov_len = sizeof(idb);
	iovcnt++;

	iov[iovcnt].iov_base = &sa->sa_hdr;
	iov[iovcnt].iov_len = sizeof(sa->sa_hdr);
	iovcnt++;
	iov[iovcnt].iov_base = &type;
	iov[iovcnt].iov_len = sizeof(type);
	iovcnt++;
	iov[iovcnt].iov_base = &more;
	iov[iovcnt].iov_len = sizeof(more);
	iovcnt++;
	if (data != NULL) {
		iov[iovcnt].iov_base = data;
		iov[iovcnt].iov_len = len;
		iovcnt++;
	}

	if (proc_composev(&env->sc_ps, procid, IMSG_CERTREQ, iov, iovcnt) == -1)
		goto done;

	sa_stateflags(sa, IKED_REQ_CERTREQ);

	ret = 0;
 done:
	ibuf_release(id.id_buf);
	return (ret);
}