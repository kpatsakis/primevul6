syn_cache_unreach(struct sockaddr *src, struct sockaddr *dst, struct tcphdr *th,
    u_int rtableid)
{
	struct syn_cache *sc;
	struct syn_cache_head *scp;

	NET_ASSERT_LOCKED();

	if ((sc = syn_cache_lookup(src, dst, &scp, rtableid)) == NULL)
		return;
	/* If the sequence number != sc_iss, then it's a bogus ICMP msg */
	if (ntohl (th->th_seq) != sc->sc_iss) {
		return;
	}

	/*
	 * If we've retransmitted 3 times and this is our second error,
	 * we remove the entry.  Otherwise, we allow it to continue on.
	 * This prevents us from incorrectly nuking an entry during a
	 * spurious network outage.
	 *
	 * See tcp_notify().
	 */
	if ((sc->sc_flags & SCF_UNREACH) == 0 || sc->sc_rxtshift < 3) {
		sc->sc_flags |= SCF_UNREACH;
		return;
	}

	syn_cache_rm(sc);
	tcpstat_inc(tcps_sc_unreach);
	syn_cache_put(sc);
}