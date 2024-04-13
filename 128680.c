txt_shift_text_currentpoint(textw_text_enum_t *penum, gs_point *wpt)
{
    return gs_moveto_aux(penum->pgs, gx_current_path(penum->pgs),
                              fixed2float(penum->origin.x) + wpt->x,
                              fixed2float(penum->origin.y) + wpt->y);
}