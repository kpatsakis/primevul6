syn_cache_put(struct syn_cache *sc)
{
	m_free(sc->sc_ipopts);
	if (sc->sc_route4.ro_rt != NULL) {
		rtfree(sc->sc_route4.ro_rt);
		sc->sc_route4.ro_rt = NULL;
	}
	timeout_set(&sc->sc_timer, syn_cache_reaper, sc);
	timeout_add(&sc->sc_timer, 0);
}