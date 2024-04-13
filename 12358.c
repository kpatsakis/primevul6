invoke_edit(
    cmdarg_T	*cap,
    int		repl,		// "r" or "gr" command
    int		cmd,
    int		startln)
{
    int		restart_edit_save = 0;

    // Complicated: When the user types "a<C-O>a" we don't want to do Insert
    // mode recursively.  But when doing "a<C-O>." or "a<C-O>rx" we do allow
    // it.
    if (repl || !stuff_empty())
	restart_edit_save = restart_edit;
    else
	restart_edit_save = 0;

    // Always reset "restart_edit", this is not a restarted edit.
    restart_edit = 0;

    if (edit(cmd, startln, cap->count1))
	cap->retval |= CA_COMMAND_BUSY;

    if (restart_edit == 0)
	restart_edit = restart_edit_save;
}