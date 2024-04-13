txt_add_sorted_fragment(gx_device_txtwrite_t *tdev, textw_text_enum_t *penum)
{
    if (!tdev->PageData.y_ordered_list) {
        /* first entry, no need to sort, just store it */
        tdev->PageData.y_ordered_list = (page_text_list_t *)gs_malloc(tdev->memory->stable_memory, 1,
            sizeof(page_text_list_t), "txtwrite alloc Y list entry");
        if (!tdev->PageData.y_ordered_list)
            return gs_note_error(gs_error_VMerror);
        memset(tdev->PageData.y_ordered_list, 0x00, sizeof(page_text_list_t));
        tdev->PageData.y_ordered_list->x_ordered_list = penum->text_state;
        tdev->PageData.y_ordered_list->next = tdev->PageData.y_ordered_list->previous = NULL;
        tdev->PageData.y_ordered_list->start = penum->text_state->start;
    } else {
        page_text_list_t *Y_List = tdev->PageData.y_ordered_list;

        while (Y_List->next && Y_List->start.y < penum->text_state->start.y)
            Y_List = Y_List->next;

        if (Y_List->start.y == penum->text_state->start.y) {
            /* Already have text at this y-position */
            text_list_entry_t *X_List = Y_List->x_ordered_list;

            while (X_List->next && X_List->start.x < penum->text_state->start.x)
                X_List = X_List->next;

            if (X_List->start.x > penum->text_state->start.x) {
                /* Insert before */
                penum->text_state->next = X_List;
                penum->text_state->previous = X_List->previous;
                X_List->previous = penum->text_state;
                if (!penum->text_state->previous)
                    /* New head of list */
                    Y_List->x_ordered_list = penum->text_state;
                else
                    penum->text_state->previous->next = penum->text_state;
            } else {
                /* Insert after */
                penum->text_state->previous = X_List;
                penum->text_state->next = X_List->next;
                X_List->next = penum->text_state;
                if (penum->text_state->next)
                    penum->text_state->next->previous = penum->text_state;
            }
            if (penum->text_state->FontBBox_bottomleft.y < Y_List->MinY)
                Y_List->MinY = penum->text_state->FontBBox_bottomleft.y;
            if (penum->text_state->FontBBox_bottomleft.y > Y_List->MaxY)
                Y_List->MaxY = penum->text_state->FontBBox_bottomleft.y;
            if (penum->text_state->FontBBox_topright.y < Y_List->MinY)
                Y_List->MinY = penum->text_state->FontBBox_topright.y;
            if (penum->text_state->FontBBox_topright.y > Y_List->MaxY)
                Y_List->MaxY = penum->text_state->FontBBox_topright.y;
        } else {
            /* New y-position, make a Y list new record */
            page_text_list_t *Y_Entry = (page_text_list_t *)gs_malloc(tdev->memory->stable_memory, 1,
                sizeof(page_text_list_t), "txtwrite alloc Y-list");
            if (!Y_Entry)
                return gs_note_error(gs_error_VMerror);

            Y_Entry->x_ordered_list = penum->text_state;
            Y_Entry->start = penum->text_state->start;
            if (penum->text_state->FontBBox_bottomleft.y > penum->text_state->FontBBox_topright.y) {
                Y_Entry->MinY = penum->text_state->FontBBox_topright.y;
                Y_Entry->MaxY = penum->text_state->FontBBox_bottomleft.y;
            } else {
                Y_Entry->MaxY = penum->text_state->FontBBox_topright.y;
                Y_Entry->MinY = penum->text_state->FontBBox_bottomleft.y;
            }

            if (Y_List->start.y > penum->text_state->start.y) {
                /* Insert before */
                Y_Entry->next = Y_List;
                Y_Entry->previous = Y_List->previous;
                Y_List->previous = Y_Entry;
                if (!Y_Entry->previous)
                    /* New head of list */
                    tdev->PageData.y_ordered_list = Y_Entry;
                else
                    ((page_text_list_t *)Y_Entry->previous)->next = Y_Entry;
            } else {
                /* Insert after */
                Y_Entry->next = Y_List->next;
                Y_Entry->previous = Y_List;
                Y_List->next = Y_Entry;
                if (Y_Entry->next)
                    ((page_text_list_t *)(Y_Entry->next))->previous = Y_Entry;
            }
        }
    }
    penum->text_state = NULL;
    return 0;
}