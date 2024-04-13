sixel_dither_set_body_only(
    sixel_dither_t /* in */ *dither,     /* dither context object */
    int            /* in */ bodyonly)    /* 0: output palette section
                                            1: do not output palette section  */
{
    dither->bodyonly = bodyonly;
}