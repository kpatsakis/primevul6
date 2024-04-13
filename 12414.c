restore_visual_mode(void)
{
    if (VIsual_mode_orig != NUL)
    {
	curbuf->b_visual.vi_mode = VIsual_mode_orig;
	VIsual_mode_orig = NUL;
    }
}