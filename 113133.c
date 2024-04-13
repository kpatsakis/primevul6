ca_setauth(struct iked *env, struct iked_sa *sa,
    struct ibuf *authmsg, enum privsep_procid id)
{
	struct iovec		 iov[3];
	int			 iovcnt = 3;
	struct iked_policy	*policy = sa->sa_policy;
	uint8_t			 type = policy->pol_auth.auth_method;

	if (id == PROC_CERT) {
		/* switch encoding to IKEV2_AUTH_SIG if SHA2 is supported */
		if (sa->sa_sigsha2 && auth_sig_compatible(type)) {
			log_debug("%s: switching %s to SIG", __func__,
			    print_map(type, ikev2_auth_map));
			type = IKEV2_AUTH_SIG;
		} else if (!sa->sa_sigsha2 && type == IKEV2_AUTH_SIG_ANY) {
			log_debug("%s: switching SIG to RSA_SIG(*)", __func__);
			/* XXX ca might auto-switch to ECDSA */
			type = IKEV2_AUTH_RSA_SIG;
		} else if (type == IKEV2_AUTH_SIG) {
			log_debug("%s: using SIG (RFC7427)", __func__);
		}
	}

	if (type == IKEV2_AUTH_SHARED_KEY_MIC) {
		sa->sa_stateflags |= IKED_REQ_AUTH;
		return (ikev2_msg_authsign(env, sa,
		    &policy->pol_auth, authmsg));
	}

	iov[0].iov_base = &sa->sa_hdr;
	iov[0].iov_len = sizeof(sa->sa_hdr);
	iov[1].iov_base = &type;
	iov[1].iov_len = sizeof(type);
	if (type == IKEV2_AUTH_NONE)
		iovcnt--;
	else {
		iov[2].iov_base = ibuf_data(authmsg);
		iov[2].iov_len = ibuf_size(authmsg);
		log_debug("%s: auth length %zu", __func__, ibuf_size(authmsg));
	}

	if (proc_composev(&env->sc_ps, id, IMSG_AUTH, iov, iovcnt) == -1)
		return (-1);
	return (0);
}