do_dialog(
    int		type UNUSED,
    char_u	*title UNUSED,
    char_u	*message,
    char_u	*buttons,
    int		dfltbutton,
    char_u	*textfield UNUSED,	// IObuff for inputdialog(), NULL
					// otherwise
    int		ex_cmd)	    // when TRUE pressing : accepts default and starts
			    // Ex command
{
    int		oldState;
    int		retval = 0;
    char_u	*hotkeys;
    int		c;
    int		i;
    tmode_T	save_tmode;

#ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
    if (silent_mode)
	return dfltbutton;   // return default option
#endif

#ifdef FEAT_GUI_DIALOG
    // When GUI is running and 'c' not in 'guioptions', use the GUI dialog
    if (gui.in_use && vim_strchr(p_go, GO_CONDIALOG) == NULL)
    {
	// --gui-dialog-file: write text to a file
	if (gui_dialog_log(title, message))
	    c = dfltbutton;
	else
	    c = gui_mch_dialog(type, title, message, buttons, dfltbutton,
							   textfield, ex_cmd);
	// avoid a hit-enter prompt without clearing the cmdline
	need_wait_return = FALSE;
	emsg_on_display = FALSE;
	cmdline_row = msg_row;

	// Flush output to avoid that further messages and redrawing is done
	// in the wrong order.
	out_flush();
	gui_mch_update();

	return c;
    }
#endif

    oldState = State;
    State = MODE_CONFIRM;
    setmouse();

    // Ensure raw mode here.
    save_tmode = cur_tmode;
    settmode(TMODE_RAW);

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = msg_show_console_dialog(message, buttons, dfltbutton);

    if (hotkeys != NULL)
    {
	for (;;)
	{
	    // Get a typed character directly from the user.
	    c = get_keystroke();
	    switch (c)
	    {
	    case CAR:		// User accepts default option
	    case NL:
		retval = dfltbutton;
		break;
	    case Ctrl_C:	// User aborts/cancels
	    case ESC:
		retval = 0;
		break;
	    default:		// Could be a hotkey?
		if (c < 0)	// special keys are ignored here
		    continue;
		if (c == ':' && ex_cmd)
		{
		    retval = dfltbutton;
		    ins_char_typebuf(':', 0);
		    break;
		}

		// Make the character lowercase, as chars in "hotkeys" are.
		c = MB_TOLOWER(c);
		retval = 1;
		for (i = 0; hotkeys[i]; ++i)
		{
		    if (has_mbyte)
		    {
			if ((*mb_ptr2char)(hotkeys + i) == c)
			    break;
			i += (*mb_ptr2len)(hotkeys + i) - 1;
		    }
		    else
			if (hotkeys[i] == c)
			    break;
		    ++retval;
		}
		if (hotkeys[i])
		    break;
		// No hotkey match, so keep waiting
		continue;
	    }
	    break;
	}

	vim_free(hotkeys);
    }

    settmode(save_tmode);
    State = oldState;
    setmouse();
    --no_wait_return;
    msg_end_prompt();

    return retval;
}