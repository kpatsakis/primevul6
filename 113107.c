ca_run(struct privsep *ps, struct privsep_proc *p, void *arg)
{
	struct iked	*env = ps->ps_env;
	struct ca_store	*store;

	/*
	 * pledge in the ca process:
	 * stdio - for malloc and basic I/O including events.
	 * rpath - for certificate files.
	 * recvfd - for ocsp sockets.
	 */
	if (pledge("stdio rpath recvfd", NULL) == -1)
		fatal("pledge");

	if ((store = calloc(1, sizeof(*store))) == NULL)
		fatal("%s: failed to allocate cert store", __func__);

	env->sc_priv = store;
	p->p_shutdown = ca_shutdown;
}