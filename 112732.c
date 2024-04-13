tcp_xmit_timer(struct tcpcb *tp, int rtt)
{
	short delta;
	short rttmin;

	if (rtt < 0)
		rtt = 0;
	else if (rtt > TCP_RTT_MAX)
		rtt = TCP_RTT_MAX;

	tcpstat_inc(tcps_rttupdated);
	if (tp->t_srtt != 0) {
		/*
		 * delta is fixed point with 2 (TCP_RTT_BASE_SHIFT) bits
		 * after the binary point (scaled by 4), whereas
		 * srtt is stored as fixed point with 5 bits after the
		 * binary point (i.e., scaled by 32).  The following magic
		 * is equivalent to the smoothing algorithm in rfc793 with
		 * an alpha of .875 (srtt = rtt/8 + srtt*7/8 in fixed
		 * point).
		 */
		delta = (rtt << TCP_RTT_BASE_SHIFT) -
		    (tp->t_srtt >> TCP_RTT_SHIFT);
		if ((tp->t_srtt += delta) <= 0)
			tp->t_srtt = 1 << TCP_RTT_BASE_SHIFT;
		/*
		 * We accumulate a smoothed rtt variance (actually, a
		 * smoothed mean difference), then set the retransmit
		 * timer to smoothed rtt + 4 times the smoothed variance.
		 * rttvar is stored as fixed point with 4 bits after the
		 * binary point (scaled by 16).  The following is
		 * equivalent to rfc793 smoothing with an alpha of .75
		 * (rttvar = rttvar*3/4 + |delta| / 4).  This replaces
		 * rfc793's wired-in beta.
		 */
		if (delta < 0)
			delta = -delta;
		delta -= (tp->t_rttvar >> TCP_RTTVAR_SHIFT);
		if ((tp->t_rttvar += delta) <= 0)
			tp->t_rttvar = 1 << TCP_RTT_BASE_SHIFT;
	} else {
		/*
		 * No rtt measurement yet - use the unsmoothed rtt.
		 * Set the variance to half the rtt (so our first
		 * retransmit happens at 3*rtt).
		 */
		tp->t_srtt = (rtt + 1) << (TCP_RTT_SHIFT + TCP_RTT_BASE_SHIFT);
		tp->t_rttvar = (rtt + 1) <<
		    (TCP_RTTVAR_SHIFT + TCP_RTT_BASE_SHIFT - 1);
	}
	tp->t_rtttime = 0;
	tp->t_rxtshift = 0;

	/*
	 * the retransmit should happen at rtt + 4 * rttvar.
	 * Because of the way we do the smoothing, srtt and rttvar
	 * will each average +1/2 tick of bias.  When we compute
	 * the retransmit timer, we want 1/2 tick of rounding and
	 * 1 extra tick because of +-1/2 tick uncertainty in the
	 * firing of the timer.  The bias will give us exactly the
	 * 1.5 tick we need.  But, because the bias is
	 * statistical, we have to test that we don't drop below
	 * the minimum feasible timer (which is 2 ticks).
	 */
	rttmin = min(max(rtt + 2, tp->t_rttmin), TCPTV_REXMTMAX);
	TCPT_RANGESET(tp->t_rxtcur, TCP_REXMTVAL(tp), rttmin, TCPTV_REXMTMAX);

	/*
	 * We received an ack for a packet that wasn't retransmitted;
	 * it is probably safe to discard any error indications we've
	 * received recently.  This isn't quite right, but close enough
	 * for now (a route might have failed after we sent a segment,
	 * and the return path might not be symmetrical).
	 */
	tp->t_softerror = 0;
}