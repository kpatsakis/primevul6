normal_cmd_get_count(
	cmdarg_T	*cap,
	int		c,
	int		toplevel UNUSED,
	int		set_prevcount UNUSED,
	int		*ctrl_w,
	int		*need_flushbuf UNUSED)
{
getcount:
    if (!(VIsual_active && VIsual_select))
    {
	// Handle a count before a command and compute ca.count0.
	// Note that '0' is a command and not the start of a count, but it's
	// part of a count after other digits.
	while ((c >= '1' && c <= '9')
		|| (cap->count0 != 0 && (c == K_DEL || c == K_KDEL
			|| c == '0')))
	{
	    if (c == K_DEL || c == K_KDEL)
	    {
		cap->count0 /= 10;
#ifdef FEAT_CMDL_INFO
		del_from_showcmd(4);	// delete the digit and ~@%
#endif
	    }
	    else if (cap->count0 > 99999999L)
	    {
		cap->count0 = 999999999L;
	    }
	    else
	    {
		cap->count0 = cap->count0 * 10 + (c - '0');
	    }
#ifdef FEAT_EVAL
	    // Set v:count here, when called from main() and not a stuffed
	    // command, so that v:count can be used in an expression mapping
	    // right after the count. Do set it for redo.
	    if (toplevel && readbuf1_empty())
		set_vcount_ca(cap, &set_prevcount);
#endif
	    if (*ctrl_w)
	    {
		++no_mapping;
		++allow_keys;		// no mapping for nchar, but keys
	    }
	    ++no_zero_mapping;		// don't map zero here
	    c = plain_vgetc();
	    LANGMAP_ADJUST(c, TRUE);
	    --no_zero_mapping;
	    if (*ctrl_w)
	    {
		--no_mapping;
		--allow_keys;
	    }
#ifdef FEAT_CMDL_INFO
	    *need_flushbuf |= add_to_showcmd(c);
#endif
	}

	// If we got CTRL-W there may be a/another count
	if (c == Ctrl_W && !*ctrl_w && cap->oap->op_type == OP_NOP)
	{
	    *ctrl_w = TRUE;
	    cap->opcount = cap->count0;	// remember first count
	    cap->count0 = 0;
	    ++no_mapping;
	    ++allow_keys;		// no mapping for nchar, but keys
	    c = plain_vgetc();		// get next character
	    LANGMAP_ADJUST(c, TRUE);
	    --no_mapping;
	    --allow_keys;
#ifdef FEAT_CMDL_INFO
	    *need_flushbuf |= add_to_showcmd(c);
#endif
	    goto getcount;		// jump back
	}
    }

    if (c == K_CURSORHOLD)
    {
	// Save the count values so that ca.opcount and ca.count0 are exactly
	// the same when coming back here after handling K_CURSORHOLD.
	cap->oap->prev_opcount = cap->opcount;
	cap->oap->prev_count0 = cap->count0;
    }
    else if (cap->opcount != 0)
    {
	// If we're in the middle of an operator (including after entering a
	// yank buffer with '"') AND we had a count before the operator, then
	// that count overrides the current value of ca.count0.
	// What this means effectively, is that commands like "3dw" get turned
	// into "d3w" which makes things fall into place pretty neatly.
	// If you give a count before AND after the operator, they are
	// multiplied.
	if (cap->count0)
	{
	    if (cap->opcount >= 999999999L / cap->count0)
		cap->count0 = 999999999L;
	    else
		cap->count0 *= cap->opcount;
	}
	else
	    cap->count0 = cap->opcount;
    }

    // Always remember the count.  It will be set to zero (on the next call,
    // above) when there is no pending operator.
    // When called from main(), save the count for use by the "count" built-in
    // variable.
    cap->opcount = cap->count0;
    cap->count1 = (cap->count0 == 0 ? 1 : cap->count0);

#ifdef FEAT_EVAL
    // Only set v:count when called from main() and not a stuffed command.
    // Do set it for redo.
    if (toplevel && readbuf1_empty())
	set_vcount(cap->count0, cap->count1, set_prevcount);
#endif

    return c;
}