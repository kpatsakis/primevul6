static int decorated_text_output(gx_device_txtwrite_t *tdev)
{
    int i;
    text_list_entry_t * x_entry, *next_x;
    char TextBuffer[512], Escaped[32];
    float xpos;
    page_text_list_t *y_list;
#ifdef TRACE_TXTWRITE
    text_list_entry_t *debug_x;
#endif

    if (tdev->TextFormat == 0) {
        gp_fwrite("<page>\n", sizeof(unsigned char), 7, tdev->file);
        x_entry = tdev->PageData.unsorted_text_list;
        while (x_entry) {
            next_x = x_entry->next;
            gs_sprintf(TextBuffer, "<span bbox=\"%0.0f %0.0f %0.0f %0.0f\" font=\"%s\" size=\"%0.4f\">\n", x_entry->start.x, x_entry->start.y,
                x_entry->end.x, x_entry->end.y, x_entry->FontName,x_entry->size);
            gp_fwrite(TextBuffer, 1, strlen(TextBuffer), tdev->file);
            xpos = x_entry->start.x;
            for (i=0;i<x_entry->Unicode_Text_Size;i++) {
                escaped_Unicode(x_entry->Unicode_Text[i], (char *)&Escaped);
                gs_sprintf(TextBuffer, "<char bbox=\"%0.0f %0.0f %0.0f %0.0f\" c=\"%s\"/>\n", xpos,
                    x_entry->start.y, xpos + x_entry->Widths[i], x_entry->end.y, Escaped);
                gp_fwrite(TextBuffer, 1, strlen(TextBuffer), tdev->file);
                xpos += x_entry->Widths[i];
            }
            gp_fwrite("</span>\n", sizeof(unsigned char), 8, tdev->file);

            x_entry = next_x;
        }
        gp_fwrite("</page>\n", sizeof(unsigned char), 8, tdev->file);
    } else {

        merge_vertically(tdev);
        merge_horizontally(tdev);

        y_list = tdev->PageData.y_ordered_list;
        gp_fwrite("<page>\n", sizeof(unsigned char), 7, tdev->file);
        /* Walk the list looking for 'blocks' */
        do {
            page_text_list_t *temp;
            page_text_t block;
            page_text_list_t *block_line;
            float BBox[4];

            memset(&block, 0x00, sizeof(page_text_t));
            memset(BBox, 0x00, sizeof(float) * 4);

            while (y_list) {
                if (block.y_ordered_list) {
                    text_list_entry_t *x_entry = y_list->x_ordered_list;

                    block_line = block.y_ordered_list;
                    while (x_entry) {
                        if (x_entry->start.x > BBox[2] || x_entry->end.x < BBox[0] ||
                            x_entry->start.y > (BBox[1] + (BBox[3] - BBox[1]))) {
                                ;
                        } else {
                            block_line->next = (page_text_list_t *)gs_malloc(tdev->memory->stable_memory, 1,
                                sizeof(page_text_list_t), "txtwrite alloc Y-list");
                            memset(block_line->next, 0x00, sizeof(page_text_list_t));
                            block_line = block_line->next;
                            block_line->x_ordered_list = x_entry;
                            if(x_entry->next)
                                x_entry->next->previous = x_entry->previous;
                            if (x_entry->previous)
                                x_entry->previous->next = x_entry->next;
                            else {
                                if (x_entry->next == 0x00) {
                                    /* remove Y entry */
                                    temp = y_list->next;
                                    if (y_list->previous)
                                        y_list->previous->next = y_list->next;
                                    if (y_list->next)
                                        y_list->next->previous = y_list->previous;
                                    else {
                                        if (y_list->previous == 0x00) {
                                            tdev->PageData.y_ordered_list = 0x00;
                                        }
                                    }
                                    gs_free(tdev->memory, y_list, 1, sizeof(page_text_list_t), "txtwrite free text list");
                                    if (tdev->PageData.y_ordered_list == y_list)
                                        tdev->PageData.y_ordered_list = temp;
                                    y_list = temp;
                                    x_entry = x_entry->next;
                                    continue;
                                }
                            }
                            if (block_line->x_ordered_list->start.x < BBox[0])
                                BBox[0] = block_line->x_ordered_list->start.x;
                            if (block_line->x_ordered_list->start.y < BBox[1])
                                BBox[1] = block_line->x_ordered_list->start.y;
                            if (block_line->x_ordered_list->end.x < BBox[2])
                                BBox[2] = block_line->x_ordered_list->end.x;
                            if (block_line->x_ordered_list->end.y + block_line->x_ordered_list->FontBBox_topright.y < BBox[3])
                                BBox[3] = block_line->x_ordered_list->end.y + block_line->x_ordered_list->FontBBox_topright.y;
                        }
                        x_entry = x_entry->next;
                    }
                } else {
                    block.y_ordered_list = block_line = (page_text_list_t *)gs_malloc(tdev->memory->stable_memory, 1,
                        sizeof(page_text_list_t), "txtwrite alloc Y-list");
                    memset(block_line, 0x00, sizeof(page_text_list_t));
                    block_line->x_ordered_list = y_list->x_ordered_list;
                    y_list->x_ordered_list = y_list->x_ordered_list->next;
                    if (y_list->x_ordered_list == 0x00) {
                        temp = y_list->next;
                        if (y_list->previous)
                            y_list->previous->next = y_list->next;
                        if (y_list->next)
                            y_list->next->previous = y_list->previous;
                        else {
                            if (y_list->previous == 0x00) {
                                tdev->PageData.y_ordered_list = 0x00;
                            }
                        }
                        gs_free(tdev->memory, y_list, 1, sizeof(page_text_list_t), "txtwrite free text list");
                        if (tdev->PageData.y_ordered_list == y_list)
                            tdev->PageData.y_ordered_list = temp;
                        y_list = temp;
                        continue;
                    }
                    block_line->x_ordered_list->next = block_line->x_ordered_list->previous = 0x00;
                    BBox[0] = block_line->x_ordered_list->start.x;
                    BBox[1] = block_line->x_ordered_list->start.y;
                    BBox[2] = block_line->x_ordered_list->end.x;
                    BBox[3] = block_line->x_ordered_list->end.y + block_line->x_ordered_list->FontBBox_topright.y;
                }
                if (y_list)
                    y_list = y_list->next;
            }
            /* FIXME - need to free the used memory in here */
            gp_fwrite("<block>\n", sizeof(unsigned char), 8, tdev->file);
            block_line = block.y_ordered_list;
            while (block_line) {
                gp_fwrite("<line>\n", sizeof(unsigned char), 7, tdev->file);
                x_entry = block_line->x_ordered_list;
                while(x_entry) {
                    gs_sprintf(TextBuffer, "<span bbox=\"%0.0f %0.0f %0.0f %0.0f\" font=\"%s\" size=\"%0.4f\">\n", x_entry->start.x, x_entry->start.y,
                        x_entry->end.x, x_entry->end.y, x_entry->FontName,x_entry->size);
                    gp_fwrite(TextBuffer, 1, strlen(TextBuffer), tdev->file);
                    xpos = x_entry->start.x;
                    for (i=0;i<x_entry->Unicode_Text_Size;i++) {
                        escaped_Unicode(x_entry->Unicode_Text[i], (char *)&Escaped);
                        gs_sprintf(TextBuffer, "<char bbox=\"%0.0f %0.0f %0.0f %0.0f\" c=\"%s\"/>\n", xpos,
                            x_entry->start.y, xpos + x_entry->Widths[i], x_entry->end.y, Escaped);
                        gp_fwrite(TextBuffer, 1, strlen(TextBuffer), tdev->file);
                        xpos += x_entry->Widths[i];
                    }
                    gp_fwrite("</span>\n", sizeof(unsigned char), 8, tdev->file);
                    x_entry = x_entry->next;
                }
                gp_fwrite("</line>\n", sizeof(unsigned char), 8, tdev->file);
                block_line = block_line->next;
            }
            gp_fwrite("</block>\n", sizeof(unsigned char), 9, tdev->file);
            y_list = tdev->PageData.y_ordered_list;
        } while (y_list);

        gp_fwrite("</page>\n", sizeof(unsigned char), 8, tdev->file);
    }
    return 0;
}