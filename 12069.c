check_quickfix_busy(void)
{
    if (quickfix_busy != 0)
    {
	semsg("quickfix_busy not zero on exit: %ld", (long)quickfix_busy);
# ifdef ABORT_ON_INTERNAL_ERROR
	abort();
# endif
    }
}