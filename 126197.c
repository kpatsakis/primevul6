load_gif(
    unsigned char       /* in */ *buffer,
    int                 /* in */ size,
    unsigned char       /* in */ *bgcolor,
    int                 /* in */ reqcolors,
    int                 /* in */ fuse_palette,
    int                 /* in */ fstatic,
    int                 /* in */ loop_control,
    void                /* in */ *fn_load,     /* callback */
    void                /* in */ *context,     /* private data for callback */
    sixel_allocator_t   /* in */ *allocator)   /* allocator object */
{
    gif_context_t s;
    gif_t g;
    SIXELSTATUS status = SIXEL_FALSE;
    sixel_frame_t *frame;
    fn_pointer fnp;

    fnp.p = fn_load;
    g.out = NULL;

    status = sixel_frame_new(&frame, allocator);
    if (SIXEL_FAILED(status)) {
        goto end;
    }
    s.img_buffer = s.img_buffer_original = (unsigned char *)buffer;
    s.img_buffer_end = (unsigned char *)buffer + size;
    memset(&g, 0, sizeof(g));
    status = gif_load_header(&s, &g);
    if (status != SIXEL_OK) {
        goto end;
    }
    g.out = (unsigned char *)sixel_allocator_malloc(allocator, (size_t)(g.w * g.h));
    if (g.out == NULL) {
        sixel_helper_set_additional_message(
            "load_gif: sixel_allocator_malloc() failed.");
        status = SIXEL_BAD_ALLOCATION;
        goto end;
    }

    frame->loop_count = 0;

    for (;;) { /* per loop */

        frame->frame_no = 0;

        s.img_buffer = s.img_buffer_original;
        status = gif_load_header(&s, &g);
        if (status != SIXEL_OK) {
            goto end;
        }

        g.is_terminated = 0;

        for (;;) { /* per frame */
            status = gif_load_next(&s, &g, bgcolor);
            if (status != SIXEL_OK) {
                goto end;
            }
            if (g.is_terminated) {
                break;
            }

            frame->width = g.w;
            frame->height = g.h;
            status = gif_init_frame(frame, &g, bgcolor, reqcolors, fuse_palette);
            if (status != SIXEL_OK) {
                goto end;
            }

            status = fnp.fn(frame, context);
            if (status != SIXEL_OK) {
                goto end;
            }

            if (fstatic) {
                goto end;
            }
            ++frame->frame_no;
        }

        ++frame->loop_count;

        if (g.loop_count < 0) {
            break;
        }
        if (loop_control == SIXEL_LOOP_DISABLE || frame->frame_no == 1) {
            break;
        }
        if (loop_control == SIXEL_LOOP_AUTO) {
            if (frame->loop_count == g.loop_count) {
                break;
            }
        }
    }

end:
    sixel_allocator_free(frame->allocator, g.out);
    sixel_frame_unref(frame);

    return status;
}