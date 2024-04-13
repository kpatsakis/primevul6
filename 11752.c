_nc_free_tparm(void)
{
#if HAVE_TSEARCH
    if (count_tparm != 0) {
	delete_tparm = typeMalloc(TPARM_DATA *, count_tparm);
	which_tparm = 0;
	twalk(cached_tparm, visit_nodes);
	for (which_tparm = 0; which_tparm < count_tparm; ++which_tparm) {
	    TPARM_DATA *ptr = delete_tparm[which_tparm];
	    tdelete(ptr, &cached_tparm, cmp_format);
	    free((char *) ptr->format);
	    free(ptr);
	}
	which_tparm = 0;
	twalk(cached_tparm, visit_nodes);
	FreeAndNull(delete_tparm);
	count_tparm = 0;
	which_tparm = 0;
    }
#endif
    FreeAndNull(TPS(out_buff));
    TPS(out_size) = 0;
    TPS(out_used) = 0;

    FreeAndNull(TPS(fmt_buff));
    TPS(fmt_size) = 0;
}