gif_init_frame(
    sixel_frame_t /* in */ *frame,
    gif_t         /* in */ *pg,
    unsigned char /* in */ *bgcolor,
    int           /* in */ reqcolors,
    int           /* in */ fuse_palette)
{
    SIXELSTATUS status = SIXEL_OK;
    int i;
    int ncolors;

    frame->delay = pg->delay;
    ncolors = 2 << (pg->flags & 7);
    if (frame->palette == NULL) {
        frame->palette = (unsigned char *)sixel_allocator_malloc(frame->allocator, (size_t)(ncolors * 3));
    } else if (frame->ncolors < ncolors) {
        sixel_allocator_free(frame->allocator, frame->palette);
        frame->palette = (unsigned char *)sixel_allocator_malloc(frame->allocator, (size_t)(ncolors * 3));
    }
    if (frame->palette == NULL) {
        sixel_helper_set_additional_message(
            "gif_init_frame: sixel_allocator_malloc() failed.");
        status = SIXEL_BAD_ALLOCATION;
        goto end;
    }
    frame->ncolors = ncolors;
    if (frame->ncolors <= reqcolors && fuse_palette) {
        frame->pixelformat = SIXEL_PIXELFORMAT_PAL8;
        sixel_allocator_free(frame->allocator, frame->pixels);
        frame->pixels = (unsigned char *)sixel_allocator_malloc(frame->allocator,
                                                                (size_t)(frame->width * frame->height));
        if (frame->pixels == NULL) {
            sixel_helper_set_additional_message(
                "sixel_allocator_malloc() failed in gif_init_frame().");
            status = SIXEL_BAD_ALLOCATION;
            goto end;
        }
        memcpy(frame->pixels, pg->out, (size_t)(frame->width * frame->height));

        for (i = 0; i < frame->ncolors; ++i) {
            frame->palette[i * 3 + 0] = pg->color_table[i * 3 + 2];
            frame->palette[i * 3 + 1] = pg->color_table[i * 3 + 1];
            frame->palette[i * 3 + 2] = pg->color_table[i * 3 + 0];
        }
        if (pg->lflags & 0x80) {
            if (pg->eflags & 0x01) {
                if (bgcolor) {
                    frame->palette[pg->transparent * 3 + 0] = bgcolor[0];
                    frame->palette[pg->transparent * 3 + 1] = bgcolor[1];
                    frame->palette[pg->transparent * 3 + 2] = bgcolor[2];
                } else {
                    frame->transparent = pg->transparent;
                }
            }
        } else if (pg->flags & 0x80) {
            if (pg->eflags & 0x01) {
                if (bgcolor) {
                    frame->palette[pg->transparent * 3 + 0] = bgcolor[0];
                    frame->palette[pg->transparent * 3 + 1] = bgcolor[1];
                    frame->palette[pg->transparent * 3 + 2] = bgcolor[2];
                } else {
                    frame->transparent = pg->transparent;
                }
            }
        }
    } else {
        frame->pixelformat = SIXEL_PIXELFORMAT_RGB888;
        frame->pixels = (unsigned char *)sixel_allocator_malloc(frame->allocator,
                                                                (size_t)(pg->w * pg->h * 3));
        if (frame->pixels == NULL) {
            sixel_helper_set_additional_message(
                "sixel_allocator_malloc() failed in gif_init_frame().");
            status = SIXEL_BAD_ALLOCATION;
            goto end;
        }
        for (i = 0; i < pg->w * pg->h; ++i) {
            frame->pixels[i * 3 + 0] = pg->color_table[pg->out[i] * 3 + 2];
            frame->pixels[i * 3 + 1] = pg->color_table[pg->out[i] * 3 + 1];
            frame->pixels[i * 3 + 2] = pg->color_table[pg->out[i] * 3 + 0];
        }
    }
    frame->multiframe = (pg->loop_count != (-1));

    status = SIXEL_OK;

end:
    return status;
}