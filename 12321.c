nv_ignore(cmdarg_T *cap)
{
    cap->retval |= CA_COMMAND_BUSY;	// don't call edit() now
}