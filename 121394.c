xtermUtf8ToTextList(XtermWidget xw,
		    XTextProperty * text_prop,
		    char ***text_list,
		    int *text_list_count)
{
    TScreen *screen = TScreenOf(xw);
    Display *dpy = screen->display;
    int rc = -1;

    if (text_prop->format == 8
	&& (rc = Xutf8TextPropertyToTextList(dpy, text_prop,
					     text_list,
					     text_list_count)) >= 0) {
	if (*text_list != NULL && *text_list_count != 0) {
	    int i;
	    Char *data;
	    char **new_text_list, *tmp;
	    unsigned long size, new_size;

	    TRACE(("xtermUtf8ToTextList size %d\n", *text_list_count));

	    /*
	     * XLib StringList actually uses only two pointers, one for the
	     * list itself, and one for the data.  Pointer to the data is the
	     * first element of the list, the rest (if any) list elements point
	     * to the same memory block as the first element
	     */
	    new_size = 0;
	    for (i = 0; i < *text_list_count; ++i) {
		data = (Char *) (*text_list)[i];
		size = strlen((*text_list)[i]) + 1;
		(void) UTF8toLatin1(screen, data, size, &size);
		new_size += size + 1;
	    }
	    new_text_list = TypeXtMallocN(char *, *text_list_count);
	    new_text_list[0] = tmp = XtMalloc((Cardinal) new_size);
	    for (i = 0; i < (*text_list_count); ++i) {
		data = (Char *) (*text_list)[i];
		size = strlen((*text_list)[i]) + 1;
		if ((data = UTF8toLatin1(screen, data, size, &size)) != 0) {
		    memcpy(tmp, data, size + 1);
		    new_text_list[i] = tmp;
		    tmp += size + 1;
		}
	    }
	    XFreeStringList((*text_list));
	    *text_list = new_text_list;
	} else {
	    rc = -1;
	}
    }
    return rc;
}