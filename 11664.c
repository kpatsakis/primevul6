tparm_tc_compat(TPARM_DATA * data)
{
    bool termcap_hack = FALSE;

    TPS(stack_ptr) = 0;

    if (data->num_popped == 0) {
	int i;

	termcap_hack = TRUE;
	for (i = data->num_parsed - 1; i >= 0; i--) {
	    if (data->p_is_s[i])
		spush(data->p_is_s[i]);
	    else
		npush((int) data->param[i]);
	}
    }
    return termcap_hack;
}