ca_dispatch_parent(int fd, struct privsep_proc *p, struct imsg *imsg)
{
	struct iked		*env = p->p_env;
	unsigned int		 mode;

	switch (imsg->hdr.type) {
	case IMSG_CTL_RESET:
		IMSG_SIZE_CHECK(imsg, &mode);
		memcpy(&mode, imsg->data, sizeof(mode));
		if (mode == RESET_ALL || mode == RESET_CA) {
			log_debug("%s: config reset", __func__);
			ca_reset(&env->sc_ps);
		}
		break;
	case IMSG_OCSP_FD:
		ocsp_receive_fd(env, imsg);
		break;
	case IMSG_OCSP_URL:
		config_getocsp(env, imsg);
		break;
	case IMSG_PRIVKEY:
	case IMSG_PUBKEY:
		config_getkey(env, imsg);
		break;
	default:
		return (-1);
	}

	return (0);
}