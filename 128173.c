msg_sb_eol(void)
{
    if (last_msgchunk != NULL)
	last_msgchunk->sb_eol = TRUE;
}