set_playmode()
{
    if (wizard) {
#ifdef WIZARD
	if (authorize_wizard_mode())
	    Strcpy(plname, "wizard");
	else
#endif
	    wizard = FALSE;	/* not allowed or not available */
	/* force explore mode if we didn't make it into wizard mode */
	discover = !wizard;
	iflags.deferred_X = FALSE;
    }
    /* don't need to do anything special for explore mode or normal play */
}