syn_cache_rm(struct syn_cache *sc)
{
	sc->sc_flags |= SCF_DEAD;
	TAILQ_REMOVE(&sc->sc_buckethead->sch_bucket, sc, sc_bucketq);
	sc->sc_tp = NULL;
	LIST_REMOVE(sc, sc_tpq);
	sc->sc_buckethead->sch_length--;
	timeout_del(&sc->sc_timer);
	sc->sc_set->scs_count--;
}