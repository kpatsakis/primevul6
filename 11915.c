cleanup(void)
{
    int rc;

#if NO_LEAKS
    free_namelist(namelst);
    _nc_leaks_dump_entry();
#endif
    if (tmp_fp != 0)
	fclose(tmp_fp);
    if (to_remove != 0) {
#if HAVE_REMOVE
	rc = remove(to_remove);
#else
	rc = unlink(to_remove);
#endif
	if (rc != 0)
	    perror(to_remove);
    }
}