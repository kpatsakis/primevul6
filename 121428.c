ClearSelectionBuffer(TScreen *screen, String selection)
{
    int which = TargetToSelection(screen, selection);
    SelectedCells *scp = &(screen->selected_cells[okSelectionCode(which)]);
    FreeAndNull(scp->data_buffer);
    scp->data_limit = 0;
    scp->data_length = 0;
    screen->base64_count = 0;
}