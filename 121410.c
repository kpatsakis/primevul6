ResetSelectionState(TScreen *screen)
{
    screen->selection_count = 0;
    screen->startH = zeroCELL;
    screen->endH = zeroCELL;
}