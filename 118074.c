initoptions()
{
	initoptions_init();
#ifdef SYSCF
	/* someday there may be other SYSCF alternatives besides text file */
# ifdef SYSCF_FILE
	read_config_file(SYSCF_FILE, SET_IN_SYS);
# endif
#endif
	initoptions_finish();
}