CompleteSelection(XtermWidget xw, String *args, Cardinal len)
{
    TScreen *screen = TScreenOf(xw);

    screen->base64_count = 0;
    screen->base64_accu = 0;
    _OwnSelection(xw, args, len);
}