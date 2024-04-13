push_showcmd(void)
{
    if (p_sc)
	STRCPY(old_showcmd_buf, showcmd_buf);
}