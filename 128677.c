static int simple_text_output(gx_device_txtwrite_t *tdev)
{
    int chars_wide;
    float char_size, min_size, min_width_size;
#ifdef TRACE_TXTWRITE
    text_list_entry_t *debug_x;
#endif
    text_list_entry_t * x_entry;
    page_text_list_t *y_list;
    unsigned short UnicodeSpace = 0x20, UnicodeEOL[2] = {0x00D, 0x0a};

    merge_vertically(tdev);

    merge_horizontally(tdev);

    min_size = (float)tdev->width;
    /* Estimate maximum text density */
    y_list = tdev->PageData.y_ordered_list;
    while (y_list) {
        x_entry = y_list->x_ordered_list;
        while (x_entry) {
            if (x_entry->size < min_size)
                min_size = x_entry->size;
            x_entry = x_entry->next;
        }
        y_list = y_list->next;
    }

    min_width_size = min_size;
    y_list = tdev->PageData.y_ordered_list;
    while (y_list) {
        float width;

        x_entry = y_list->x_ordered_list;
        while (x_entry) {
            width = (x_entry->end.x - x_entry->start.x) / x_entry->Unicode_Text_Size;
            if (x_entry->next) {
                /* If we have following text, check to see if *not* using the newly calculated size would result in
                 * the end of the text going past the beginning of the following text. If it does then we must
                 * use the new minimum, regardless of how small it is! The foregoing comment isn't quite true...
                 * we never used a width of 0 because that would result in an endless loop, we need to allow a little
                 * slop in case rounding errors mean that the x difference from start to end of the text is almost,
                 * but not quite, zero.
                 */
                if (x_entry->start.x + ((x_entry->Unicode_Text_Size + 1) * min_width_size) > x_entry->next->start.x && width > 0.001)
                    min_width_size = width;
            } else {
                if (width < min_width_size && width >= (float)min_size * 0.75)
                    min_width_size = width;
            }
            x_entry = x_entry->next;
        }
        y_list = y_list->next;
    }

    min_size = min_width_size;
    chars_wide = (int)ceil(tdev->width / min_size);
    char_size = (float)tdev->width / (float)chars_wide;

    y_list = tdev->PageData.y_ordered_list;
    while (y_list) {
        float xpos = 0;
        x_entry = y_list->x_ordered_list;
        while (x_entry) {
            while (xpos < x_entry->start.x) {
                write_simple_text(&UnicodeSpace, 1, tdev);
                xpos += char_size;
            }
            write_simple_text(x_entry->Unicode_Text, x_entry->Unicode_Text_Size, tdev);
            xpos += x_entry->Unicode_Text_Size * char_size;
            if (x_entry->next) {
                x_entry = x_entry->next;
            } else {
                x_entry = NULL;
            }
        }
        write_simple_text((unsigned short *)&UnicodeEOL, 2, tdev);
        if (y_list->next) {
            y_list = y_list->next;
        } else {
            y_list = NULL;
        }
    }
    return 0;
}