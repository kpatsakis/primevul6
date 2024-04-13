tcp_sack_partialack(struct tcpcb *tp, struct tcphdr *th)
{
	/* Turn off retx. timer (will start again next segment) */
	TCP_TIMER_DISARM(tp, TCPT_REXMT);
	tp->t_rtttime = 0;
	/*
	 * Partial window deflation.  This statement relies on the
	 * fact that tp->snd_una has not been updated yet.
	 */
	if (tp->snd_cwnd > (th->th_ack - tp->snd_una)) {
		tp->snd_cwnd -= th->th_ack - tp->snd_una;
		tp->snd_cwnd += tp->t_maxseg;
	} else
		tp->snd_cwnd = tp->t_maxseg;
	tp->snd_cwnd += tp->t_maxseg;
	tp->t_flags |= TF_NEEDOUTPUT;
}