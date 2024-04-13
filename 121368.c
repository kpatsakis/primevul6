beginInternalSelect(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    InternalSelect *mydata = &(screen->internal_select);

    (void) mydata;
    /* override flags so that SelectionReceived only updates a buffer */
#if OPT_PASTE64
    mydata->base64_paste = screen->base64_paste;
    screen->base64_paste = 0;
#endif
#if OPT_PASTE64 || OPT_READLINE
    mydata->paste_brackets = screen->paste_brackets;
    SCREEN_FLAG_unset(screen, paste_brackets);
#endif
}