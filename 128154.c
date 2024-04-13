other_sourcing_name(void)
{
    if (HAVE_SOURCING_INFO && SOURCING_NAME != NULL)
    {
	if (last_sourcing_name != NULL)
	    return STRCMP(SOURCING_NAME, last_sourcing_name) != 0;
	return TRUE;
    }
    return FALSE;
}