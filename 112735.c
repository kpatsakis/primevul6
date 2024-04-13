tcp_mss_adv(struct mbuf *m, int af)
{
	int mss = 0;
	int iphlen;
	struct ifnet *ifp = NULL;

	if (m && (m->m_flags & M_PKTHDR))
		ifp = if_get(m->m_pkthdr.ph_ifidx);

	switch (af) {
	case AF_INET:
		if (ifp != NULL)
			mss = ifp->if_mtu;
		iphlen = sizeof(struct ip);
		break;
#ifdef INET6
	case AF_INET6:
		if (ifp != NULL)
			mss = ifp->if_mtu;
		iphlen = sizeof(struct ip6_hdr);
		break;
#endif  
	default:
		unhandled_af(af);
	}
	if_put(ifp);
	mss = mss - iphlen - sizeof(struct tcphdr);
	return (max(mss, tcp_mssdflt));
}