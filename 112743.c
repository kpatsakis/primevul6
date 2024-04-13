syn_cache_timer(void *arg)
{
	struct syn_cache *sc = arg;

	NET_LOCK();
	if (sc->sc_flags & SCF_DEAD)
		goto out;

	if (__predict_false(sc->sc_rxtshift == TCP_MAXRXTSHIFT)) {
		/* Drop it -- too many retransmissions. */
		goto dropit;
	}

	/*
	 * Compute the total amount of time this entry has
	 * been on a queue.  If this entry has been on longer
	 * than the keep alive timer would allow, expire it.
	 */
	sc->sc_rxttot += sc->sc_rxtcur;
	if (sc->sc_rxttot >= tcptv_keep_init)
		goto dropit;

	tcpstat_inc(tcps_sc_retransmitted);
	(void) syn_cache_respond(sc, NULL);

	/* Advance the timer back-off. */
	sc->sc_rxtshift++;
	SYN_CACHE_TIMER_ARM(sc);

 out:
	NET_UNLOCK();
	return;

 dropit:
	tcpstat_inc(tcps_sc_timed_out);
	syn_cache_rm(sc);
	syn_cache_put(sc);
	NET_UNLOCK();
}