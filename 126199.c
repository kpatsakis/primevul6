gif_load_header(
    gif_context_t /* in */ *s,
    gif_t         /* in */ *g)
{
    SIXELSTATUS status = SIXEL_FALSE;
    unsigned char version;
    if (gif_get8(s) != 'G') {
        goto end;
    }
    if (gif_get8(s) != 'I') {
        goto end;
    }
    if (gif_get8(s) != 'F') {
        goto end;
    }
    if (gif_get8(s) != '8') {
        goto end;
    }

    version = gif_get8(s);

    if (version != '7' && version != '9') {
        goto end;
    }
    if (gif_get8(s) != 'a') {
        goto end;
    }

    g->w = gif_get16le(s);
    g->h = gif_get16le(s);
    g->flags = gif_get8(s);
    g->bgindex = gif_get8(s);
    g->ratio = gif_get8(s);
    g->transparent = (-1);
    g->loop_count = (-1);

    if (g->flags & 0x80) {
        gif_parse_colortable(s, g->pal, 2 << (g->flags & 7));
    }

    status = SIXEL_OK;

end:
    return status;
}