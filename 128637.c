txtwrite_text_begin(gx_device * dev, gs_gstate * pgs,
                const gs_text_params_t * text, gs_font * font,
                gx_path * path, const gx_device_color * pdcolor,
                const gx_clip_path * pcpath,
                gs_memory_t * mem, gs_text_enum_t ** ppenum)
{
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;
    textw_text_enum_t *penum;
    int code;

    /* If this is a stringwidth, we must let the default graphics library code handle it
     * in case there is no current point (this can happen if this is the first operation
     * we get, the current font is a CIDFont, and its descendant is a substitute type 1
     * font). If there is no current point we throw an error in text_process and that
     * messes up all the font handling. The test below is copied from pdfwrite
     * (gdev_pdf_text_begin) and seems to do the job.
     */
    if ((!(text->operation & TEXT_DO_DRAW) && pgs->text_rendering_mode != 3)
                    || path == 0 || !path_position_valid(path))
            return gx_default_text_begin(dev, pgs, text, font, path, pdcolor,
                                         pcpath, mem, ppenum);
    /* Allocate and initialize one of our text enumerators. */
    rc_alloc_struct_1(penum, textw_text_enum_t, &st_textw_text_enum, mem,
                      return_error(gs_error_VMerror), "gdev_textw_text_begin");
    penum->rc.free = rc_free_text_enum;
    penum->charproc_accum = false;
    penum->cdevproc_callout = false;
    penum->returned.total_width.x = penum->returned.total_width.y = 0;
    penum->TextBuffer = NULL;
    penum->TextBufferIndex = 0;
    penum->Widths = NULL;
    /* The enumerator's text_release method frees this memory */
    penum->text_state = (text_list_entry_t *)gs_malloc(tdev->memory->stable_memory, 1,
            sizeof(text_list_entry_t), "txtwrite alloc text state");
    if (!penum->text_state)
        return gs_note_error(gs_error_VMerror);
    memset(penum->text_state, 0x00, sizeof(text_list_entry_t));

    code = gs_text_enum_init((gs_text_enum_t *)penum, &textw_text_procs,
                             dev, pgs, text, font, path, pdcolor, pcpath, mem);
    if (code < 0) {
        /* Belt and braces; I'm not certain this is required, but its safe */
        gs_free(tdev->memory, penum->text_state, 1, sizeof(text_list_entry_t), "txtwrite free text state");
        penum->text_state = NULL;
        gs_free_object(mem, penum, "textwrite_text_begin");
        return code;
    }

    code = gx_path_current_point(penum->path, &penum->origin);
    if (code != 0)
       return code;

    *ppenum = (gs_text_enum_t *)penum;

    return 0;
}