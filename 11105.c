set_tmx(VALUE self, struct tmx *tmx)
{
    get_d1(self);
    tmx->dat = (void *)dat;
    tmx->funcs = &tmx_funcs;
}