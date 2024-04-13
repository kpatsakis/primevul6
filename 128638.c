static int merge_horizontally(gx_device_txtwrite_t *tdev)
{
#ifdef TRACE_TXTWRITE
    text_list_entry_t *debug_x;
#endif
    unsigned short UnicodeSpace = 0x20;
    page_text_list_t *y_list = tdev->PageData.y_ordered_list;

    while (y_list) {
        float average_width;
        text_list_entry_t *from, *to;
        from = y_list->x_ordered_list;
        to = from->next;

        while (from && to) {
            average_width = (from->end.x - from->start.x) / from->Unicode_Text_Size;

            if (to->start.x - from->end.x < average_width / 2) {
                /* consolidate fragments */
                unsigned short *NewText;
                float *NewWidths;

                NewText = (unsigned short *)gs_malloc(tdev->memory->stable_memory,
                    (from->Unicode_Text_Size + to->Unicode_Text_Size), sizeof(unsigned short), "txtwrite alloc working text buffer");
                NewWidths = (float *)gs_malloc(tdev->memory->stable_memory,
                    (from->Unicode_Text_Size + to->Unicode_Text_Size), sizeof(float), "txtwrite alloc Widths array");
                if (!NewText || !NewWidths) {
                    if (NewText)
                        gs_free(tdev->memory, NewText, from->Unicode_Text_Size + to->Unicode_Text_Size, sizeof (unsigned short), "free working text fragment");
                    /* ran out of memory, don't consolidate */
                    from = from->next;
                    to = to->next;
                } else {
#ifdef TRACE_TXTWRITE
                    gp_fprintf(tdev->DebugFile, "Consolidating two horizontal fragments in one line, before:\n\t");
                    gp_fwrite(from->Unicode_Text, sizeof(unsigned short), from->Unicode_Text_Size, tdev->DebugFile);
                    gp_fprintf(tdev->DebugFile, "\n\t");
                    gp_fwrite(to->Unicode_Text, sizeof(unsigned short), to->Unicode_Text_Size, tdev->DebugFile);
#endif
                    memcpy(NewText, from->Unicode_Text, from->Unicode_Text_Size * sizeof(unsigned short));
                    memcpy(&NewText[from->Unicode_Text_Size], to->Unicode_Text, to->Unicode_Text_Size * sizeof(unsigned short));
                    memcpy(NewWidths, from->Widths, from->Unicode_Text_Size * sizeof(float));
                    memcpy(&NewWidths[from->Unicode_Text_Size], to->Widths, to->Unicode_Text_Size * sizeof(float));
                    gs_free(tdev->memory, from->Unicode_Text, from->Unicode_Text_Size, sizeof (unsigned short), "free consolidated text fragment");
                    gs_free(tdev->memory, to->Unicode_Text, to->Unicode_Text_Size, sizeof (unsigned short), "free consolidated text fragment");
                    gs_free(tdev->memory, from->Widths, from->Unicode_Text_Size, sizeof (float), "free consolidated Widths array");
                    gs_free(tdev->memory, to->Widths, to->Unicode_Text_Size, sizeof (float), "free consolidated Widths array");
                    gs_free(tdev->memory, to->FontName, 1, strlen(from->FontName) + 1, "free FontName");
                    from->Unicode_Text = NewText;
                    from->Unicode_Text_Size += to->Unicode_Text_Size;
                    from->Widths = NewWidths;
#ifdef TRACE_TXTWRITE
                    gp_fprintf(tdev->DebugFile, "After:\n\t");
                    gp_fwrite(from->Unicode_Text, sizeof(unsigned short), from->Unicode_Text_Size, tdev->DebugFile);
#endif
                    from->end = to->end;
                    from->next = to->next;
                    if (from->next)
                        from->next->previous = from;
                    gs_free(tdev->memory, to, 1, sizeof(text_list_entry_t), "free consolidated fragment");
                    to = from->next;
                }
            } else {
                if (to->start.x - from->end.x < average_width *2){
                    unsigned short *NewText;
                    float *NewWidths;

                    NewText = (unsigned short *)gs_malloc(tdev->memory->stable_memory,
                        (from->Unicode_Text_Size + to->Unicode_Text_Size + 1), sizeof(unsigned short), "txtwrite alloc text state");
                    NewWidths = (float *)gs_malloc(tdev->memory->stable_memory,
                        (from->Unicode_Text_Size + to->Unicode_Text_Size + 1), sizeof(float), "txtwrite alloc Widths array");
                    if (!NewText || !NewWidths) {
                        if (NewText)
                            gs_free(tdev->memory, NewText, from->Unicode_Text_Size + to->Unicode_Text_Size, sizeof (unsigned short), "free working text fragment");
                        /* ran out of memory, don't consolidate */
                        from = from->next;
                        to = to->next;
                    } else {
                        memcpy(NewText, from->Unicode_Text, from->Unicode_Text_Size * sizeof(unsigned short));
                        memcpy(&NewText[from->Unicode_Text_Size], &UnicodeSpace, sizeof(unsigned short));
                        memcpy(&NewText[from->Unicode_Text_Size + 1], to->Unicode_Text, to->Unicode_Text_Size * sizeof(unsigned short));
                        memcpy(NewWidths, from->Widths, from->Unicode_Text_Size * sizeof(float));
                        NewWidths[from->Unicode_Text_Size] = to->start.x - from->end.x;
                        memcpy(&NewWidths[from->Unicode_Text_Size + 1], to->Widths, to->Unicode_Text_Size * sizeof(float));
                        gs_free(tdev->memory, from->Unicode_Text, from->Unicode_Text_Size, sizeof (unsigned short), "free consolidated text fragment");
                        gs_free(tdev->memory, to->Unicode_Text, to->Unicode_Text_Size, sizeof (unsigned short), "free consolidated text fragment");
                        gs_free(tdev->memory, from->Widths, from->Unicode_Text_Size, sizeof (float), "free consolidated Widths array");
                        gs_free(tdev->memory, to->Widths, to->Unicode_Text_Size, sizeof (float), "free consolidated Widths array");
                        gs_free(tdev->memory, to->FontName, 1, strlen(from->FontName) + 1, "free FontName");
                        from->Unicode_Text = NewText;
                        from->Unicode_Text_Size += to->Unicode_Text_Size + 1;
                        from->Widths = NewWidths;
                        from->end = to->end;
                        from->next = to->next;
                        if (from->next)
                            from->next->previous = from;
                        gs_free(tdev->memory, to, 1, sizeof(text_list_entry_t), "free consolidated fragment");
                        to = from->next;
                    }
                } else {
                    from = from->next;
                    to = to->next;
                }
            }
        }
        y_list = y_list->next;
    }
    return 0;
}