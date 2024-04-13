nv_down(cmdarg_T *cap)
{
    if (mod_mask & MOD_MASK_SHIFT)
    {
	// <S-Down> is page down
	cap->arg = FORWARD;
	nv_page(cap);
    }
#if defined(FEAT_QUICKFIX)
    // Quickfix window only: view the result under the cursor.
    else if (bt_quickfix(curbuf) && cap->cmdchar == CAR)
	qf_view_result(FALSE);
#endif
    else
    {
#ifdef FEAT_CMDWIN
	// In the cmdline window a <CR> executes the command.
	if (cmdwin_type != 0 && cap->cmdchar == CAR)
	    cmdwin_result = CAR;
	else
#endif
#ifdef FEAT_JOB_CHANNEL
	// In a prompt buffer a <CR> in the last line invokes the callback.
	if (bt_prompt(curbuf) && cap->cmdchar == CAR
		       && curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	{
	    invoke_prompt_callback();
	    if (restart_edit == 0)
		restart_edit = 'a';
	}
	else
#endif
	{
	    cap->oap->motion_type = MLINE;
	    if (cursor_down(cap->count1, cap->oap->op_type == OP_NOP) == FAIL)
		clearopbeep(cap->oap);
	    else if (cap->arg)
		beginline(BL_WHITE | BL_FIX);
	}
    }
}