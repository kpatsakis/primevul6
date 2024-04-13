rndset(unsigned long v)
{
	register uint32_t h;
#if defined(arc4random_pushb_fast) || defined(MKSH_A4PB)
	register uint32_t t;
#endif
	struct {
		struct timeval tv;
		void *sp;
		uint32_t qh;
		pid_t pp;
		short r;
	} z;

#ifdef DEBUG
	/* clear the allocated space, for valgrind */
	memset(&z, 0, sizeof(z));
#endif

	h = lcg_state;
	BAFHFinish_reg(h);
	BAFHUpdateMem_reg(h, &v, sizeof(v));

	mksh_TIME(z.tv);
	z.sp = &lcg_state;
	z.pp = procpid;
	z.r = (short)rndget();

#if defined(arc4random_pushb_fast) || defined(MKSH_A4PB)
	t = qh_state;
	BAFHFinish_reg(t);
	z.qh = (t & 0xFFFF8000) | rndget();
	lcg_state = (t << 15) | rndget();
	/*
	 * either we have very chap entropy get and push available,
	 * with malloc() pulling in this code already anyway, or the
	 * user requested us to use the old functions
	 */
	t = h;
	BAFHUpdateMem_reg(t, &lcg_state, sizeof(lcg_state));
	BAFHFinish_reg(t);
	lcg_state = t;
#if defined(arc4random_pushb_fast)
	arc4random_pushb_fast(&lcg_state, sizeof(lcg_state));
	lcg_state = arc4random();
#else
	lcg_state = arc4random_pushb(&lcg_state, sizeof(lcg_state));
#endif
	BAFHUpdateMem_reg(h, &lcg_state, sizeof(lcg_state));
#else
	z.qh = qh_state;
#endif

	BAFHUpdateMem_reg(h, &z, sizeof(z));
	BAFHFinish_reg(h);
	lcg_state = h;
}