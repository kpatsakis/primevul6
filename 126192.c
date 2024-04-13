gif_parse_colortable(
    gif_context_t /* in */ *s,
    unsigned char       /* in */ pal[256][3],
    int           /* in */ num_entries)
{
    int i;

    for (i = 0; i < num_entries; ++i) {
        pal[i][2] = gif_get8(s);
        pal[i][1] = gif_get8(s);
        pal[i][0] = gif_get8(s);
    }
}