static int merge_vertically(gx_device_txtwrite_t *tdev)
{
#ifdef TRACE_TXTWRITE
    text_list_entry_t *debug_x;
#endif
    page_text_list_t *y_list = tdev->PageData.y_ordered_list;

    while (y_list && y_list->next) {
        page_text_list_t *next = y_list->next;
        bool collision = false;
        float overlap = (y_list->start.y + y_list->MaxY) - (next->start.y + next->MinY);

        if (overlap >= (y_list->MaxY - y_list->MinY) / 4) {
            /* At least a 25% overlap, lets test for x collisions */
            text_list_entry_t *upper = y_list->x_ordered_list, *lower;
            while (upper && !collision) {
                lower = next->x_ordered_list;
                while (lower && !collision) {
                    if (upper->start.x >= lower->start.x) {
                        if (upper->start.x <= lower->end.x) {
                            /* Collision */
                            collision = true;
                            break;
                        }
                    } else {
                        if (upper->end.x > lower->start.x) {
                            /* Collision */
                            collision = true;
                            break;
                        }
                    }
                    lower = lower->next;
                }
                upper = upper->next;
            }
            if (!collision) {
                text_list_entry_t *from, *to, *new_order, *current;
                /* Consolidate y lists */
                to = y_list->x_ordered_list;
                from = next->x_ordered_list;
#ifdef TRACE_TXTWRITE
                fprintf(tdev->DebugFile, "\nConsolidating two horizontal lines, line 1:");
                debug_x = from;
                while (debug_x) {
                    gp_fprintf(tdev->DebugFile, "\n\t");
                    gp_fwrite(debug_x->Unicode_Text, sizeof(unsigned short), debug_x->Unicode_Text_Size, tdev->DebugFile);
                    debug_x = debug_x->next;
                }
                fprintf(tdev->DebugFile, "\nConsolidating two horizontal lines, line 2");
                debug_x = to;
                while (debug_x) {
                    gp_fprintf(tdev->DebugFile, "\n\t");
                    gp_fwrite(debug_x->Unicode_Text, sizeof(unsigned short), debug_x->Unicode_Text_Size, tdev->DebugFile);
                    debug_x = debug_x->next;
                }
#endif
                if (from->start.x < to->start.x) {
                    current = new_order = from;
                    from = from->next;
                } else {
                    current = new_order = to;
                    to = to->next;
                }
                while (to && from) {
                    if (to->start.x < from->start.x) {
                        current->next = to;
                        to->previous = current;
                        to = to->next;
                    } else {
                        current->next = from;
                        from->previous = current;
                        from = from->next;
                    }
                    current = current->next;
                }
                if (to) {
                    to->previous = current;
                    current->next = to;
                } else {
                    if (from) {
                        from->previous = current;
                        current->next = from;
                    }
                }
                y_list->x_ordered_list = new_order;
#ifdef TRACE_TXTWRITE
                fprintf(tdev->DebugFile, "\nAfter:");
                debug_x = new_order;
                while (debug_x) {
                    gp_fprintf(tdev->DebugFile, "\n\t");
                    gp_fwrite(debug_x->Unicode_Text, sizeof(unsigned short), debug_x->Unicode_Text_Size, tdev->DebugFile);
                    debug_x = debug_x->next;
                }
                fprintf(tdev->DebugFile, "\n");
#endif
                y_list->next = next->next;
                if (next->next)
                    next->next->previous = y_list;
                gs_free(tdev->memory, next, 1, sizeof(page_text_list_entry_t), "txtwrite free text list");
            } else
                y_list = next;
        } else
            y_list = next;
    }
    return 0;
}