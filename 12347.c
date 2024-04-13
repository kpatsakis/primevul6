nv_drop(cmdarg_T *cap UNUSED)
{
    do_put('~', NULL, BACKWARD, 1L, PUT_CURSEND);
}