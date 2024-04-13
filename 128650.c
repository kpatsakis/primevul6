txt_add_fragment(gx_device_txtwrite_t *tdev, textw_text_enum_t *penum)
{
    text_list_entry_t *unsorted_entry, *t;

    /* Create a duplicate entry for the unsorted list */
    unsorted_entry = (text_list_entry_t *)gs_malloc(tdev->memory->stable_memory, 1,
            sizeof(text_list_entry_t), "txtwrite alloc sorted text state");
    if (!unsorted_entry)
        return gs_note_error(gs_error_VMerror);

    /* Calculate the start and end points of the text */
    penum->text_state->start.x = fixed2float(penum->origin.x);
    penum->text_state->start.y = fixed2float(penum->origin.y);
    penum->text_state->end.x = penum->text_state->start.x + penum->returned.total_width.x;
    penum->text_state->end.y = penum->text_state->start.y + penum->returned.total_width.y;
    penum->text_state->Unicode_Text_Size = penum->TextBufferIndex;

    *unsorted_entry = *(penum->text_state);

    /* Update the saved text state with the acccumulated Unicode data */
    /* The working buffer (penum->TextBuffer) is freed in the text_release method */
    penum->text_state->Unicode_Text = (unsigned short *)gs_malloc(tdev->memory->stable_memory,
        penum->TextBufferIndex, sizeof(unsigned short), "txtwrite alloc text buffer");
    if (!penum->text_state->Unicode_Text)
        return gs_note_error(gs_error_VMerror);
    memcpy(penum->text_state->Unicode_Text, penum->TextBuffer, penum->TextBufferIndex * sizeof(unsigned short));

    penum->text_state->Widths = (float *)gs_malloc(tdev->memory->stable_memory,
        penum->TextBufferIndex, sizeof(float), "txtwrite alloc widths array");
    if (!penum->text_state->Widths)
        return gs_note_error(gs_error_VMerror);
    memcpy(penum->text_state->Widths, penum->Widths, penum->TextBufferIndex * sizeof(float));

    unsorted_entry->Unicode_Text = (unsigned short *)gs_malloc(tdev->memory->stable_memory,
        penum->TextBufferIndex, sizeof(unsigned short), "txtwrite alloc sorted text buffer");
    if (!unsorted_entry->Unicode_Text)
        return gs_note_error(gs_error_VMerror);
    memcpy(unsorted_entry->Unicode_Text, penum->TextBuffer, penum->TextBufferIndex * sizeof(unsigned short));

    unsorted_entry->Widths = (float *)gs_malloc(tdev->memory->stable_memory,
        penum->TextBufferIndex, sizeof(float), "txtwrite alloc widths array");
    if (!unsorted_entry->Widths)
        return gs_note_error(gs_error_VMerror);
    memcpy(unsorted_entry->Widths, penum->Widths, penum->TextBufferIndex * sizeof(float));

    unsorted_entry->FontName = (char *)gs_malloc(tdev->memory->stable_memory,
        (strlen(penum->text_state->FontName) + 1), sizeof(unsigned char), "txtwrite alloc sorted text buffer");
    if (!unsorted_entry->FontName)
        return gs_note_error(gs_error_VMerror);
    memcpy(unsorted_entry->FontName, penum->text_state->FontName, strlen(penum->text_state->FontName) * sizeof(unsigned char));
    unsorted_entry->FontName[strlen(penum->text_state->FontName)] = 0x00;

    /* First add one entry to the unsorted list */
    if (!tdev->PageData.unsorted_text_list) {
        tdev->PageData.unsorted_text_list = unsorted_entry;
        unsorted_entry->next = unsorted_entry->previous = NULL;
    } else {
        t = tdev->PageData.unsorted_text_list;
        while (t->next)
            t = t->next;
        t->next = unsorted_entry;
        unsorted_entry->next = NULL;
        unsorted_entry->previous = t;
    }

    /* Then add the other entry to the sorted list */
    return txt_add_sorted_fragment(tdev, penum);
}