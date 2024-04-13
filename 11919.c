_nc_has_mouse(SCREEN *sp)
{
    return (((0 == sp) || (sp->_mouse_type == M_NONE)) ? FALSE : TRUE);
}