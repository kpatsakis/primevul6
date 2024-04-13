ca_reset(struct privsep *ps)
{
	struct iked	*env = ps->ps_env;
	struct ca_store	*store = env->sc_priv;

	if (store->ca_privkey.id_type == IKEV2_ID_NONE ||
	    store->ca_pubkey.id_type == IKEV2_ID_NONE)
		fatalx("ca_reset: keys not loaded");

	if (store->ca_cas != NULL)
		X509_STORE_free(store->ca_cas);
	if (store->ca_certs != NULL)
		X509_STORE_free(store->ca_certs);

	if ((store->ca_cas = X509_STORE_new()) == NULL)
		fatalx("ca_reset: failed to get ca store");
	if ((store->ca_calookup = X509_STORE_add_lookup(store->ca_cas,
	    X509_LOOKUP_file())) == NULL)
		fatalx("ca_reset: failed to add ca lookup");

	if ((store->ca_certs = X509_STORE_new()) == NULL)
		fatalx("ca_reset: failed to get cert store");
	if ((store->ca_certlookup = X509_STORE_add_lookup(store->ca_certs,
	    X509_LOOKUP_file())) == NULL)
		fatalx("ca_reset: failed to add cert lookup");

	if (ca_reload(env) != 0)
		fatal("ca_reset: reload");
}