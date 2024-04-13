qf_store_title(qf_list_T *qfl, char_u *title)
{
    VIM_CLEAR(qfl->qf_title);

    if (title != NULL)
    {
	char_u *p = alloc_id(STRLEN(title) + 2, aid_qf_title);

	qfl->qf_title = p;
	if (p != NULL)
	    STRCPY(p, title);
    }
}