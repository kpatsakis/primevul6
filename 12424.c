set_vcount_ca(cmdarg_T *cap, int *set_prevcount)
{
    long count = cap->count0;

    // multiply with cap->opcount the same way as above
    if (cap->opcount != 0)
	count = cap->opcount * (count == 0 ? 1 : count);
    set_vcount(count, count == 0 ? 1 : count, *set_prevcount);
    *set_prevcount = FALSE;  // only set v:prevcount once
}