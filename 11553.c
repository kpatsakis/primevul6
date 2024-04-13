tparm_copy_valist(TPARM_DATA * data, int use_TPARM_ARG, va_list ap)
{
    int i;

    for (i = 0; i < data->num_actual; i++) {
	if (data->p_is_s[i] != 0) {
	    char *value = va_arg(ap, char *);
	    if (value == 0)
		value = dummy;
	    data->p_is_s[i] = value;
	    data->param[i] = 0;
	} else if (use_TPARM_ARG) {
	    data->param[i] = va_arg(ap, TPARM_ARG);
	} else {
	    data->param[i] = (TPARM_ARG) va_arg(ap, int);
	}
    }
}