tcp_mss_update(struct tcpcb *tp)
{
	int mss;
	u_long bufsize;
	struct rtentry *rt;
	struct socket *so;

	so = tp->t_inpcb->inp_socket;
	mss = tp->t_maxseg;

	rt = in_pcbrtentry(tp->t_inpcb);

	if (rt == NULL)
		return;

	bufsize = so->so_snd.sb_hiwat;
	if (bufsize < mss) {
		mss = bufsize;
		/* Update t_maxseg and t_maxopd */
		tcp_mss(tp, mss);
	} else {
		bufsize = roundup(bufsize, mss);
		if (bufsize > sb_max)
			bufsize = sb_max;
		(void)sbreserve(so, &so->so_snd, bufsize);
	}

	bufsize = so->so_rcv.sb_hiwat;
	if (bufsize > mss) {
		bufsize = roundup(bufsize, mss);
		if (bufsize > sb_max)
			bufsize = sb_max;
		(void)sbreserve(so, &so->so_rcv, bufsize);
	}

}