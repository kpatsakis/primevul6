syn_cache_cleanup(struct tcpcb *tp)
{
	struct syn_cache *sc, *nsc;

	NET_ASSERT_LOCKED();

	LIST_FOREACH_SAFE(sc, &tp->t_sc, sc_tpq, nsc) {
#ifdef DIAGNOSTIC
		if (sc->sc_tp != tp)
			panic("invalid sc_tp in syn_cache_cleanup");
#endif
		syn_cache_rm(sc);
		syn_cache_put(sc);
	}
	/* just for safety */
	LIST_INIT(&tp->t_sc);
}