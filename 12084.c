ll_free_all(qf_info_T **pqi)
{
    int		i;
    qf_info_T	*qi;

    qi = *pqi;
    if (qi == NULL)
	return;
    *pqi = NULL;	// Remove reference to this list

    // If the location list is still in use, then queue the delete request
    // to be processed later.
    if (quickfix_busy > 0)
    {
	locstack_queue_delreq(qi);
	return;
    }

    qi->qf_refcount--;
    if (qi->qf_refcount < 1)
    {
	// No references to this location list.
	// If the quickfix window buffer is loaded, then wipe it
	wipe_qf_buffer(qi);

	for (i = 0; i < qi->qf_listcount; ++i)
	    qf_free(qf_get_list(qi, i));
	vim_free(qi);
    }
}