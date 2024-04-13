txtwrite_output_page(gx_device * dev, int num_copies, int flush)
{
    int code;
    gx_device_txtwrite_t *const tdev = (gx_device_txtwrite_t *) dev;
    text_list_entry_t * x_entry, *next_x;
    page_text_list_t *y_list;
    gs_parsed_file_name_t parsed;
    const char *fmt;
    const short BOM = 0xFEFF;

    if (!tdev->file) {
        /* Either this is the first page, or we're doing one file per page */
        code = gx_device_open_output_file(dev, tdev->fname,
                true, false, &tdev->file); /* binary, sequential */
        if (code < 0)
            return code;
    }

    switch(tdev->TextFormat) {
        case 0:
        case 1:
            code = decorated_text_output(tdev);
            if (code < 0)
                return code;
            break;

        case 2:
            gp_fwrite (&BOM, sizeof(unsigned short), 1, tdev->file);
            /* fall through */
        case 3:
            code = simple_text_output(tdev);
            if (code < 0)
                return code;
            break;

        default:
            return gs_note_error(gs_error_rangecheck);
            break;
    }

    code =  gx_default_output_page(dev, num_copies, flush);
    if (code < 0)
        return code;

    /* free the sorted fragment list! */
    y_list = tdev->PageData.y_ordered_list;
    while (y_list) {
        x_entry = y_list->x_ordered_list;
        while (x_entry) {
            gs_free(tdev->memory, x_entry->Unicode_Text, x_entry->Unicode_Text_Size, sizeof (usnigned short), "txtwrite free text fragment text buffer");
            gs_free(tdev->memory, x_entry->Widths, x_entry->Unicode_Text_Size, sizeof (float), "txtwrite free widths array");
            gs_free(tdev->memory, x_entry->FontName, 1, strlen(x_entry->FontName) + 1, "txtwrite free Font Name");
            if (x_entry->next) {
                x_entry = x_entry->next;
                gs_free(tdev->memory, x_entry->previous, 1, sizeof(text_list_entry_t), "txtwrite free text fragment");
            } else {
                gs_free(tdev->memory, x_entry, 1, sizeof(text_list_entry_t), "txtwrite free text fragment");
                x_entry = NULL;
            }
        }
        if (y_list->next) {
            y_list = y_list->next;
            gs_free(tdev->memory, y_list->previous, 1, sizeof(page_text_list_t), "txtwrite free text list");
        } else {
            gs_free(tdev->memory, y_list, 1, sizeof(page_text_list_t), "txtwrite free text list");
            y_list = NULL;
        }
    }
    tdev->PageData.y_ordered_list = NULL;

    /* free the unsorted fragment list */
    x_entry = tdev->PageData.unsorted_text_list;
    while (x_entry) {
        next_x = x_entry->next;
        gs_free(tdev->memory, x_entry->Unicode_Text, x_entry->Unicode_Text_Size, sizeof (usnigned short), "txtwrite free unsorted text fragment text buffer");
        gs_free(tdev->memory, x_entry->Widths, x_entry->Unicode_Text_Size, sizeof (float), "txtwrite free widths array");
        gs_free(tdev->memory, x_entry->FontName, 1, strlen(x_entry->FontName) + 1, "txtwrite free Font Name");
        gs_free(tdev->memory, x_entry, 1, sizeof(text_list_entry_t), "txtwrite free unsorted text fragment");
        x_entry = next_x;
    }
    tdev->PageData.unsorted_text_list = NULL;

    code = gx_parse_output_file_name(&parsed, &fmt, tdev->fname,
                                         strlen(tdev->fname), tdev->memory);

    if (code >= 0 && fmt) { /* file per page */
        code = gx_device_close_output_file(dev, tdev->fname, tdev->file);
        tdev->file = NULL;
    }
    return code;
}