reset_VIsual_and_resel(void)
{
    if (VIsual_active)
    {
	end_visual_mode();
	redraw_curbuf_later(INVERTED);	// delete the inversion later
    }
    VIsual_reselect = FALSE;
}