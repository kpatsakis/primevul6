qf_set_title_var(qf_list_T *qfl)
{
    if (qfl->qf_title != NULL)
	set_internal_string_var((char_u *)"w:quickfix_title", qfl->qf_title);
}