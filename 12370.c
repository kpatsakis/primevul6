nv_scroll_line(cmdarg_T *cap)
{
    if (!checkclearop(cap->oap))
	scroll_redraw(cap->arg, cap->count1);
}