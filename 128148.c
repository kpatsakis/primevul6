message_filtered(char_u *msg)
{
    int match;

    if (cmdmod.cmod_filter_regmatch.regprog == NULL)
	return FALSE;
    match = vim_regexec(&cmdmod.cmod_filter_regmatch, msg, (colnr_T)0);
    return cmdmod.cmod_filter_force ? match : !match;
}