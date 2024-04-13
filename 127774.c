static void sl_free_bufs(struct slip *sl)
{
	/* Free all SLIP frame buffers. */
	kfree(xchg(&sl->rbuff, NULL));
	kfree(xchg(&sl->xbuff, NULL));
#ifdef SL_INCLUDE_CSLIP
	kfree(xchg(&sl->cbuff, NULL));
	slhc_free(xchg(&sl->slcomp, NULL));
#endif
}