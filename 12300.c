pop_showcmd(void)
{
    if (!p_sc)
	return;

    STRCPY(showcmd_buf, old_showcmd_buf);

    display_showcmd();
}