nv_cursorhold(cmdarg_T *cap)
{
    apply_autocmds(EVENT_CURSORHOLD, NULL, NULL, FALSE, curbuf);
    did_cursorhold = TRUE;
    cap->retval |= CA_COMMAND_BUSY;	// don't call edit() now
}