delete_first_msg(void)
{
    struct msg_hist *p;

    if (msg_hist_len <= 0)
	return FAIL;
    p = first_msg_hist;
    first_msg_hist = p->next;
    if (first_msg_hist == NULL)
	last_msg_hist = NULL;  // history is empty
    vim_free(p->msg);
    vim_free(p);
    --msg_hist_len;
    return OK;
}