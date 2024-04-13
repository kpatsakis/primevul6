syn_cache_reset(struct sockaddr *src, struct sockaddr *dst, struct tcphdr *th,
    u_int rtableid)
{
	struct syn_cache *sc;
	struct syn_cache_head *scp;

	NET_ASSERT_LOCKED();

	if ((sc = syn_cache_lookup(src, dst, &scp, rtableid)) == NULL)
		return;
	if (SEQ_LT(th->th_seq, sc->sc_irs) ||
	    SEQ_GT(th->th_seq, sc->sc_irs + 1))
		return;
	syn_cache_rm(sc);
	tcpstat_inc(tcps_sc_reset);
	syn_cache_put(sc);
}