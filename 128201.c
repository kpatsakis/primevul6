do_more_prompt(int typed_char)
{
    static int	entered = FALSE;
    int		used_typed_char = typed_char;
    int		oldState = State;
    int		c;
#ifdef FEAT_CON_DIALOG
    int		retval = FALSE;
#endif
    int		toscroll;
    msgchunk_T	*mp_last = NULL;
    msgchunk_T	*mp;
    int		i;

    // We get called recursively when a timer callback outputs a message. In
    // that case don't show another prompt. Also when at the hit-Enter prompt
    // and nothing was typed.
    if (entered || (State == MODE_HITRETURN && typed_char == 0))
	return FALSE;
    entered = TRUE;

    if (typed_char == 'G')
    {
	// "g<": Find first line on the last page.
	mp_last = msg_sb_start(last_msgchunk);
	for (i = 0; i < Rows - 2 && mp_last != NULL
					     && mp_last->sb_prev != NULL; ++i)
	    mp_last = msg_sb_start(mp_last->sb_prev);
    }

    State = MODE_ASKMORE;
    setmouse();
    if (typed_char == NUL)
	msg_moremsg(FALSE);
    for (;;)
    {
	/*
	 * Get a typed character directly from the user.
	 */
	if (used_typed_char != NUL)
	{
	    c = used_typed_char;	// was typed at hit-enter prompt
	    used_typed_char = NUL;
	}
	else
	    c = get_keystroke();

#if defined(FEAT_MENU) && defined(FEAT_GUI)
	if (c == K_MENU)
	{
	    int idx = get_menu_index(current_menu, MODE_ASKMORE);

	    // Used a menu.  If it starts with CTRL-Y, it must
	    // be a "Copy" for the clipboard.  Otherwise
	    // assume that we end
	    if (idx == MENU_INDEX_INVALID)
		continue;
	    c = *current_menu->strings[idx];
	    if (c != NUL && current_menu->strings[idx][1] != NUL)
		ins_typebuf(current_menu->strings[idx] + 1,
				current_menu->noremap[idx], 0, TRUE,
						   current_menu->silent[idx]);
	}
#endif

	toscroll = 0;
	switch (c)
	{
	case BS:		// scroll one line back
	case K_BS:
	case 'k':
	case K_UP:
	    toscroll = -1;
	    break;

	case CAR:		// one extra line
	case NL:
	case 'j':
	case K_DOWN:
	    toscroll = 1;
	    break;

	case 'u':		// Up half a page
	    toscroll = -(Rows / 2);
	    break;

	case 'd':		// Down half a page
	    toscroll = Rows / 2;
	    break;

	case 'b':		// one page back
	case K_PAGEUP:
	    toscroll = -(Rows - 1);
	    break;

	case ' ':		// one extra page
	case 'f':
	case K_PAGEDOWN:
	case K_LEFTMOUSE:
	    toscroll = Rows - 1;
	    break;

	case 'g':		// all the way back to the start
	    toscroll = -999999;
	    break;

	case 'G':		// all the way to the end
	    toscroll = 999999;
	    lines_left = 999999;
	    break;

	case ':':		// start new command line
#ifdef FEAT_CON_DIALOG
	    if (!confirm_msg_used)
#endif
	    {
		// Since got_int is set all typeahead will be flushed, but we
		// want to keep this ':', remember that in a special way.
		typeahead_noflush(':');
#ifdef FEAT_TERMINAL
		skip_term_loop = TRUE;
#endif
		cmdline_row = Rows - 1;		// put ':' on this line
		skip_redraw = TRUE;		// skip redraw once
		need_wait_return = FALSE;	// don't wait in main()
	    }
	    // FALLTHROUGH
	case 'q':		// quit
	case Ctrl_C:
	case ESC:
#ifdef FEAT_CON_DIALOG
	    if (confirm_msg_used)
	    {
		// Jump to the choices of the dialog.
		retval = TRUE;
	    }
	    else
#endif
	    {
		got_int = TRUE;
		quit_more = TRUE;
	    }
	    // When there is some more output (wrapping line) display that
	    // without another prompt.
	    lines_left = Rows - 1;
	    break;

#ifdef FEAT_CLIPBOARD
	case Ctrl_Y:
	    // Strange way to allow copying (yanking) a modeless
	    // selection at the more prompt.  Use CTRL-Y,
	    // because the same is used in Cmdline-mode and at the
	    // hit-enter prompt.  However, scrolling one line up
	    // might be expected...
	    if (clip_star.state == SELECT_DONE)
		clip_copy_modeless_selection(TRUE);
	    continue;
#endif
	default:		// no valid response
	    msg_moremsg(TRUE);
	    continue;
	}

	if (toscroll != 0)
	{
	    if (toscroll < 0)
	    {
		// go to start of last line
		if (mp_last == NULL)
		    mp = msg_sb_start(last_msgchunk);
		else if (mp_last->sb_prev != NULL)
		    mp = msg_sb_start(mp_last->sb_prev);
		else
		    mp = NULL;

		// go to start of line at top of the screen
		for (i = 0; i < Rows - 2 && mp != NULL && mp->sb_prev != NULL;
									  ++i)
		    mp = msg_sb_start(mp->sb_prev);

		if (mp != NULL && mp->sb_prev != NULL)
		{
		    // Find line to be displayed at top.
		    for (i = 0; i > toscroll; --i)
		    {
			if (mp == NULL || mp->sb_prev == NULL)
			    break;
			mp = msg_sb_start(mp->sb_prev);
			if (mp_last == NULL)
			    mp_last = msg_sb_start(last_msgchunk);
			else
			    mp_last = msg_sb_start(mp_last->sb_prev);
		    }

		    if (toscroll == -1 && screen_ins_lines(0, 0, 1,
						     (int)Rows, 0, NULL) == OK)
		    {
			// display line at top
			(void)disp_sb_line(0, mp);
		    }
		    else
		    {
			// redisplay all lines
			screenclear();
			for (i = 0; mp != NULL && i < Rows - 1; ++i)
			{
			    mp = disp_sb_line(i, mp);
			    ++msg_scrolled;
			}
		    }
		    toscroll = 0;
		}
	    }
	    else
	    {
		// First display any text that we scrolled back.
		while (toscroll > 0 && mp_last != NULL)
		{
		    // scroll up, display line at bottom
		    msg_scroll_up();
		    inc_msg_scrolled();
		    screen_fill((int)Rows - 2, (int)Rows - 1, 0,
						   (int)Columns, ' ', ' ', 0);
		    mp_last = disp_sb_line((int)Rows - 2, mp_last);
		    --toscroll;
		}
	    }

	    if (toscroll <= 0)
	    {
		// displayed the requested text, more prompt again
		screen_fill((int)Rows - 1, (int)Rows, 0,
						   (int)Columns, ' ', ' ', 0);
		msg_moremsg(FALSE);
		continue;
	    }

	    // display more text, return to caller
	    lines_left = toscroll;
	}

	break;
    }

    // clear the --more-- message
    screen_fill((int)Rows - 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);
    State = oldState;
    setmouse();
    if (quit_more)
    {
	msg_row = Rows - 1;
	msg_col = 0;
    }
#ifdef FEAT_RIGHTLEFT
    else if (cmdmsg_rl)
	msg_col = Columns - 1;
#endif

    entered = FALSE;
#ifdef FEAT_CON_DIALOG
    return retval;
#else
    return FALSE;
#endif
}