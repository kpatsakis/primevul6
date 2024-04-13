m_min(union DateData *x)
{
    if (simple_dat_p(x))
	return 0;
    else {
	get_c_time(x);
#ifndef USE_PACK
	return x->c.min;
#else
	return EX_MIN(x->c.pc);
#endif
    }
}