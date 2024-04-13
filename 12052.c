decr_quickfix_busy(void)
{
    if (--quickfix_busy == 0)
    {
	// No longer referencing the location lists. Process all the pending
	// delete requests.
	while (qf_delq_head != NULL)
	{
	    qf_delq_T	*q = qf_delq_head;

	    qf_delq_head = q->next;
	    ll_free_all(&q->qi);
	    vim_free(q);
	}
    }
#ifdef ABORT_ON_INTERNAL_ERROR
    if (quickfix_busy < 0)
    {
	emsg("quickfix_busy has become negative");
	abort();
    }
#endif
}