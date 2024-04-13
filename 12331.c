nv_ver_scrollbar(cmdarg_T *cap)
{
    if (cap->oap->op_type != OP_NOP)
	clearopbeep(cap->oap);

    // Even if an operator was pending, we still want to scroll
    gui_do_scroll();
}