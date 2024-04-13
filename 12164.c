locstack_queue_delreq(qf_info_T *qi)
{
    qf_delq_T	*q;

    q = ALLOC_ONE(qf_delq_T);
    if (q != NULL)
    {
	q->qi = qi;
	q->next = qf_delq_head;
	qf_delq_head = q;
    }
}