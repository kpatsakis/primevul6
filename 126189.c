gif_load_next(
    gif_context_t /* in */ *s,
    gif_t         /* in */ *g,
    unsigned char /* in */ *bgcolor
)
{
    SIXELSTATUS status = SIXEL_FALSE;
    unsigned char buffer[256];
    int x;
    int y;
    int w;
    int h;
    int len;

    for (;;) {
        switch (gif_get8(s)) {
        case 0x2C: /* Image Descriptor */
            x = gif_get16le(s);
            y = gif_get16le(s);
            w = gif_get16le(s);
            h = gif_get16le(s);
            if (((x + w) > (g->w)) || ((y + h) > (g->h))) {
                sixel_helper_set_additional_message(
                    "corrupt GIF (reason: bad Image Descriptor).");
                status = SIXEL_RUNTIME_ERROR;
                goto end;
            }

            g->line_size = g->w;
            g->start_x = x;
            g->start_y = y * g->line_size;
            g->max_x   = g->start_x + w;
            g->max_y   = g->start_y + h * g->line_size;
            g->cur_x   = g->start_x;
            g->cur_y   = g->start_y;

            g->lflags = gif_get8(s);

            if (g->lflags & 0x40) {
                g->step = 8 * g->line_size; /* first interlaced spacing */
                g->parse = 3;
            } else {
                g->step = g->line_size;
                g->parse = 0;
            }

            if (g->lflags & 0x80) {
                gif_parse_colortable(s,
                                     g->lpal,
                                     2 << (g->lflags & 7));
                g->color_table = (unsigned char *) g->lpal;
            } else if (g->flags & 0x80) {
                if (g->transparent >= 0 && (g->eflags & 0x01)) {
                   if (bgcolor) {
                       g->pal[g->transparent][0] = bgcolor[2];
                       g->pal[g->transparent][1] = bgcolor[1];
                       g->pal[g->transparent][2] = bgcolor[0];
                   }
                }
                g->color_table = (unsigned char *)g->pal;
            } else {
                sixel_helper_set_additional_message(
                    "corrupt GIF (reason: missing color table).");
                status = SIXEL_RUNTIME_ERROR;
                goto end;
            }

            status = gif_process_raster(s, g);
            if (SIXEL_FAILED(status)) {
                goto end;
            }
            goto end;

        case 0x21: /* Comment Extension. */
            switch (gif_get8(s)) {
            case 0x01: /* Plain Text Extension */
                break;
            case 0x21: /* Comment Extension */
                break;
            case 0xF9: /* Graphic Control Extension */
                len = gif_get8(s); /* block size */
                if (len == 4) {
                    g->eflags = gif_get8(s);
                    g->delay = gif_get16le(s); /* delay */
                    g->transparent = gif_get8(s);
                } else {
                    s->img_buffer += len;
                    break;
                }
                break;
            case 0xFF: /* Application Extension */
                len = gif_get8(s); /* block size */
                if (s->img_buffer + len > s->img_buffer_end) {
                    status = SIXEL_RUNTIME_ERROR;
                    goto end;
                }
                memcpy(buffer, s->img_buffer, (size_t)len);
                s->img_buffer += len;
                buffer[len] = 0;
                if (len == 11 && strcmp((char *)buffer, "NETSCAPE2.0") == 0) {
                    if (gif_get8(s) == 0x03) {
                        /* loop count */
                        switch (gif_get8(s)) {
                        case 0x00:
                            g->loop_count = 1;
                            break;
                        case 0x01:
                            g->loop_count = gif_get16le(s);
                            break;
                        default:
                            g->loop_count = 1;
                            break;
                        }
                    }
                }
                break;
            default:
                break;
            }
            while ((len = gif_get8(s)) != 0) {
                s->img_buffer += len;
            }
            break;

        case 0x3B: /* gif stream termination code */
            g->is_terminated = 1;
            status = SIXEL_OK;
            goto end;

        default:
            sixel_helper_set_additional_message(
                "corrupt GIF (reason: unknown code).");
            status = SIXEL_RUNTIME_ERROR;
            goto end;
        }
    }

    status = SIXEL_OK;

end:
    return status;
}