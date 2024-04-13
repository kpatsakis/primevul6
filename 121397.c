finishInternalSelect(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    InternalSelect *mydata = &(screen->internal_select);

    (void) mydata;
#if OPT_PASTE64
    screen->base64_paste = mydata->base64_paste;
#endif
#if OPT_PASTE64 || OPT_READLINE
    screen->paste_brackets = mydata->paste_brackets;
#endif
}