ca_shutdown(struct privsep_proc *p)
{
	struct iked             *env = p->p_env;
	struct ca_store		*store;

	if (env == NULL)
		return;
	ibuf_release(env->sc_certreq);
	if ((store = env->sc_priv) == NULL)
		return;
	ibuf_release(store->ca_pubkey.id_buf);
	ibuf_release(store->ca_privkey.id_buf);
	free(store);
}