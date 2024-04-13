define_gs_font(i_ctx_t *i_ctx_p, gs_font * pfont)
{
    return (pfont->base == pfont && pfont->dir == 0 ?
            /* i.e., unregistered original font */
            gs_definefont(ifont_dir, pfont) :
            0);
}