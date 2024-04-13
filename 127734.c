chvt_rndsetup(const void *bp, size_t sz)
{
	register uint32_t h;

	/* use LCG as seed but try to get them to deviate immediately */
	h = lcg_state;
	(void)rndget();
	BAFHFinish_reg(h);
	/* variation through pid, ppid, and the works */
	BAFHUpdateMem_reg(h, &rndsetupstate, sizeof(rndsetupstate));
	/* some variation, some possibly entropy, depending on OE */
	BAFHUpdateMem_reg(h, bp, sz);
	/* mix them all up */
	BAFHFinish_reg(h);

	return (h);
}