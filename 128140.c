verbose_stop(void)
{
    if (verbose_fd != NULL)
    {
	fclose(verbose_fd);
	verbose_fd = NULL;
    }
    verbose_did_open = FALSE;
}