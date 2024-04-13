check_visual_highlight(void)
{
    static int	    did_check = FALSE;

    if (full_screen)
    {
	if (!did_check && HL_ATTR(HLF_V) == 0)
	    msg(_("Warning: terminal cannot highlight"));
	did_check = TRUE;
    }
}