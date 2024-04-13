give_warning2(char_u *message, char_u *a1, int hl)
{
    if (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just give
	// the raw message so the user at least gets a hint.
	give_warning(message, hl);
    }
    else
    {
	vim_snprintf((char *)IObuff, IOSIZE, (char *)message, a1);
	give_warning(IObuff, hl);
    }
}