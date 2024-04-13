syn_cache_get(struct sockaddr *src, struct sockaddr *dst, struct tcphdr *th,
    u_int hlen, u_int tlen, struct socket *so, struct mbuf *m)
{
	struct syn_cache *sc;
	struct syn_cache_head *scp;
	struct inpcb *inp, *oldinp;
	struct tcpcb *tp = NULL;
	struct mbuf *am;
	struct socket *oso;

	NET_ASSERT_LOCKED();

	sc = syn_cache_lookup(src, dst, &scp, sotoinpcb(so)->inp_rtableid);
	if (sc == NULL)
		return (NULL);

	/*
	 * Verify the sequence and ack numbers.  Try getting the correct
	 * response again.
	 */
	if ((th->th_ack != sc->sc_iss + 1) ||
	    SEQ_LEQ(th->th_seq, sc->sc_irs) ||
	    SEQ_GT(th->th_seq, sc->sc_irs + 1 + sc->sc_win)) {
		(void) syn_cache_respond(sc, m);
		return ((struct socket *)(-1));
	}

	/* Remove this cache entry */
	syn_cache_rm(sc);

	/*
	 * Ok, create the full blown connection, and set things up
	 * as they would have been set up if we had created the
	 * connection when the SYN arrived.  If we can't create
	 * the connection, abort it.
	 */
	oso = so;
	so = sonewconn(so, SS_ISCONNECTED);
	if (so == NULL)
		goto resetandabort;

	oldinp = sotoinpcb(oso);
	inp = sotoinpcb(so);

#ifdef IPSEC
	/*
	 * We need to copy the required security levels
	 * from the old pcb. Ditto for any other
	 * IPsec-related information.
	 */
	memcpy(inp->inp_seclevel, oldinp->inp_seclevel,
	    sizeof(oldinp->inp_seclevel));
#endif /* IPSEC */
#ifdef INET6
	/*
	 * inp still has the OLD in_pcb stuff, set the
	 * v6-related flags on the new guy, too.
	 */
	inp->inp_flags |= (oldinp->inp_flags & INP_IPV6);
	if (inp->inp_flags & INP_IPV6) {
		inp->inp_ipv6.ip6_hlim = oldinp->inp_ipv6.ip6_hlim;
		inp->inp_hops = oldinp->inp_hops;
	} else
#endif /* INET6 */
	{
		inp->inp_ip.ip_ttl = oldinp->inp_ip.ip_ttl;
	}

#if NPF > 0
	if (m->m_pkthdr.pf.flags & PF_TAG_DIVERTED) {
		struct pf_divert *divert;

		divert = pf_find_divert(m);
		KASSERT(divert != NULL);
		inp->inp_rtableid = divert->rdomain;
	} else
#endif
	/* inherit rtable from listening socket */
	inp->inp_rtableid = sc->sc_rtableid;

	inp->inp_lport = th->th_dport;
	switch (src->sa_family) {
#ifdef INET6
	case AF_INET6:
		inp->inp_laddr6 = satosin6(dst)->sin6_addr;
		break;
#endif /* INET6 */
	case AF_INET:
		inp->inp_laddr = satosin(dst)->sin_addr;
		inp->inp_options = ip_srcroute(m);
		if (inp->inp_options == NULL) {
			inp->inp_options = sc->sc_ipopts;
			sc->sc_ipopts = NULL;
		}
		break;
	}
	in_pcbrehash(inp);

	/*
	 * Give the new socket our cached route reference.
	 */
	if (src->sa_family == AF_INET)
		inp->inp_route = sc->sc_route4;         /* struct assignment */
#ifdef INET6
	else
		inp->inp_route6 = sc->sc_route6;
#endif
	sc->sc_route4.ro_rt = NULL;

	am = m_get(M_DONTWAIT, MT_SONAME);	/* XXX */
	if (am == NULL)
		goto resetandabort;
	am->m_len = src->sa_len;
	memcpy(mtod(am, caddr_t), src, src->sa_len);
	if (in_pcbconnect(inp, am)) {
		(void) m_free(am);
		goto resetandabort;
	}
	(void) m_free(am);

	tp = intotcpcb(inp);
	tp->t_flags = sototcpcb(oso)->t_flags & (TF_NOPUSH|TF_NODELAY);
	if (sc->sc_request_r_scale != 15) {
		tp->requested_s_scale = sc->sc_requested_s_scale;
		tp->request_r_scale = sc->sc_request_r_scale;
		tp->t_flags |= TF_REQ_SCALE|TF_RCVD_SCALE;
	}
	if (sc->sc_flags & SCF_TIMESTAMP)
		tp->t_flags |= TF_REQ_TSTMP|TF_RCVD_TSTMP;

	tp->t_template = tcp_template(tp);
	if (tp->t_template == 0) {
		tp = tcp_drop(tp, ENOBUFS);	/* destroys socket */
		so = NULL;
		goto abort;
	}
	tp->sack_enable = sc->sc_flags & SCF_SACK_PERMIT;
	tp->ts_modulate = sc->sc_modulate;
	tp->ts_recent = sc->sc_timestamp;
	tp->iss = sc->sc_iss;
	tp->irs = sc->sc_irs;
	tcp_sendseqinit(tp);
	tp->snd_last = tp->snd_una;
#ifdef TCP_ECN
	if (sc->sc_flags & SCF_ECN_PERMIT) {
		tp->t_flags |= TF_ECN_PERMIT;
		tcpstat_inc(tcps_ecn_accepts);
	}
#endif
	if (sc->sc_flags & SCF_SACK_PERMIT)
		tp->t_flags |= TF_SACK_PERMIT;
#ifdef TCP_SIGNATURE
	if (sc->sc_flags & SCF_SIGNATURE)
		tp->t_flags |= TF_SIGNATURE;
#endif
	tcp_rcvseqinit(tp);
	tp->t_state = TCPS_SYN_RECEIVED;
	tp->t_rcvtime = tcp_now;
	TCP_TIMER_ARM(tp, TCPT_KEEP, tcptv_keep_init);
	tcpstat_inc(tcps_accepts);

	tcp_mss(tp, sc->sc_peermaxseg);	 /* sets t_maxseg */
	if (sc->sc_peermaxseg)
		tcp_mss_update(tp);
	/* Reset initial window to 1 segment for retransmit */
	if (sc->sc_rxtshift > 0)
		tp->snd_cwnd = tp->t_maxseg;
	tp->snd_wl1 = sc->sc_irs;
	tp->rcv_up = sc->sc_irs + 1;

	/*
	 * This is what whould have happened in tcp_output() when
	 * the SYN,ACK was sent.
	 */
	tp->snd_up = tp->snd_una;
	tp->snd_max = tp->snd_nxt = tp->iss+1;
	TCP_TIMER_ARM(tp, TCPT_REXMT, tp->t_rxtcur);
	if (sc->sc_win > 0 && SEQ_GT(tp->rcv_nxt + sc->sc_win, tp->rcv_adv))
		tp->rcv_adv = tp->rcv_nxt + sc->sc_win;
	tp->last_ack_sent = tp->rcv_nxt;

	tcpstat_inc(tcps_sc_completed);
	syn_cache_put(sc);
	return (so);

resetandabort:
	tcp_respond(NULL, mtod(m, caddr_t), th, (tcp_seq)0, th->th_ack, TH_RST,
	    m->m_pkthdr.ph_rtableid);
abort:
	m_freem(m);
	if (so != NULL)
		(void) soabort(so);
	syn_cache_put(sc);
	tcpstat_inc(tcps_sc_aborted);
	return ((struct socket *)(-1));
}