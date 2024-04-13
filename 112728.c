tcp_mss(struct tcpcb *tp, int offer)
{
	struct rtentry *rt;
	struct ifnet *ifp = NULL;
	int mss, mssopt;
	int iphlen;
	struct inpcb *inp;

	inp = tp->t_inpcb;

	mssopt = mss = tcp_mssdflt;

	rt = in_pcbrtentry(inp);

	if (rt == NULL)
		goto out;

	ifp = if_get(rt->rt_ifidx);
	if (ifp == NULL)
		goto out;

	switch (tp->pf) {
#ifdef INET6
	case AF_INET6:
		iphlen = sizeof(struct ip6_hdr);
		break;
#endif
	case AF_INET:
		iphlen = sizeof(struct ip);
		break;
	default:
		/* the family does not support path MTU discovery */
		goto out;
	}

	/*
	 * if there's an mtu associated with the route and we support
	 * path MTU discovery for the underlying protocol family, use it.
	 */
	if (rt->rt_mtu) {
		/*
		 * One may wish to lower MSS to take into account options,
		 * especially security-related options.
		 */
		if (tp->pf == AF_INET6 && rt->rt_mtu < IPV6_MMTU) {
			/*
			 * RFC2460 section 5, last paragraph: if path MTU is
			 * smaller than 1280, use 1280 as packet size and
			 * attach fragment header.
			 */
			mss = IPV6_MMTU - iphlen - sizeof(struct ip6_frag) -
			    sizeof(struct tcphdr);
		} else {
			mss = rt->rt_mtu - iphlen -
			    sizeof(struct tcphdr);
		}
	} else if (ifp->if_flags & IFF_LOOPBACK) {
		mss = ifp->if_mtu - iphlen - sizeof(struct tcphdr);
	} else if (tp->pf == AF_INET) {
		if (ip_mtudisc)
			mss = ifp->if_mtu - iphlen - sizeof(struct tcphdr);
	}
#ifdef INET6
	else if (tp->pf == AF_INET6) {
		/*
		 * for IPv6, path MTU discovery is always turned on,
		 * or the node must use packet size <= 1280.
		 */
		mss = ifp->if_mtu - iphlen - sizeof(struct tcphdr);
	}
#endif /* INET6 */

	/* Calculate the value that we offer in TCPOPT_MAXSEG */
	if (offer != -1) {
		mssopt = ifp->if_mtu - iphlen - sizeof(struct tcphdr);
		mssopt = max(tcp_mssdflt, mssopt);
	}
 out:
	if_put(ifp);
	/*
	 * The current mss, t_maxseg, is initialized to the default value.
	 * If we compute a smaller value, reduce the current mss.
	 * If we compute a larger value, return it for use in sending
	 * a max seg size option, but don't store it for use
	 * unless we received an offer at least that large from peer.
	 *
	 * However, do not accept offers lower than the minimum of
	 * the interface MTU and 216.
	 */
	if (offer > 0)
		tp->t_peermss = offer;
	if (tp->t_peermss)
		mss = min(mss, max(tp->t_peermss, 216));

	/* sanity - at least max opt. space */
	mss = max(mss, 64);

	/*
	 * maxopd stores the maximum length of data AND options
	 * in a segment; maxseg is the amount of data in a normal
	 * segment.  We need to store this value (maxopd) apart
	 * from maxseg, because now every segment carries options
	 * and thus we normally have somewhat less data in segments.
	 */
	tp->t_maxopd = mss;

	if ((tp->t_flags & (TF_REQ_TSTMP|TF_NOOPT)) == TF_REQ_TSTMP &&
	    (tp->t_flags & TF_RCVD_TSTMP) == TF_RCVD_TSTMP)
		mss -= TCPOLEN_TSTAMP_APPA;
#ifdef TCP_SIGNATURE
	if (tp->t_flags & TF_SIGNATURE)
		mss -= TCPOLEN_SIGLEN;
#endif

	if (offer == -1) {
		/* mss changed due to Path MTU discovery */
		tp->t_flags &= ~TF_PMTUD_PEND;
		tp->t_pmtud_mtu_sent = 0;
		tp->t_pmtud_mss_acked = 0;
		if (mss < tp->t_maxseg) {
			/*
			 * Follow suggestion in RFC 2414 to reduce the
			 * congestion window by the ratio of the old
			 * segment size to the new segment size.
			 */
			tp->snd_cwnd = ulmax((tp->snd_cwnd / tp->t_maxseg) *
					     mss, mss);
		}
	} else if (tcp_do_rfc3390 == 2) {
		/* increase initial window  */
		tp->snd_cwnd = ulmin(10 * mss, ulmax(2 * mss, 14600));
	} else if (tcp_do_rfc3390) {
		/* increase initial window  */
		tp->snd_cwnd = ulmin(4 * mss, ulmax(2 * mss, 4380));
	} else
		tp->snd_cwnd = mss;

	tp->t_maxseg = mss;

	return (offer != -1 ? mssopt : mss);
}