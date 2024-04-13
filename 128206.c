msg_scroll_up(void)
{
#ifdef FEAT_GUI
    // Remove the cursor before scrolling, ScreenLines[] is going
    // to become invalid.
    if (gui.in_use)
	gui_undraw_cursor();
#endif
    // scrolling up always works
    mch_disable_flush();
    screen_del_lines(0, 0, 1, (int)Rows, TRUE, 0, NULL);
    mch_enable_flush();

    if (!can_clear((char_u *)" "))
    {
	// Scrolling up doesn't result in the right background.  Set the
	// background here.  It's not efficient, but avoids that we have to do
	// it all over the code.
	screen_fill((int)Rows - 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);

	// Also clear the last char of the last but one line if it was not
	// cleared before to avoid a scroll-up.
	if (ScreenAttrs[LineOffset[Rows - 2] + Columns - 1] == (sattr_T)-1)
	    screen_fill((int)Rows - 2, (int)Rows - 1,
				 (int)Columns - 1, (int)Columns, ' ', ' ', 0);
    }
}