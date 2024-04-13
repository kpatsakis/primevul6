nv_tagpop(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
	do_tag((char_u *)"", DT_POP, (int)cap->count1, FALSE, TRUE);
}