SelectionReceived(Widget w,
		  XtPointer client_data,
		  Atom *selection GCC_UNUSED,
		  Atom *type,
		  XtPointer value,
		  unsigned long *length,
		  int *format)
{
    char **text_list = NULL;
    int text_list_count = 0;
    XTextProperty text_prop;
    TScreen *screen;
    Display *dpy;
#if OPT_TRACE && OPT_WIDE_CHARS
    Char *line = (Char *) value;
#endif

    XtermWidget xw;

    if ((xw = getXtermWidget(w)) == 0)
	return;

    screen = TScreenOf(xw);
    dpy = XtDisplay(w);

    if (*type == 0		/*XT_CONVERT_FAIL */
	|| *length == 0
	|| value == NULL) {
	TRACE(("...no data to convert\n"));
	goto fail;
    }

    text_prop.value = (unsigned char *) value;
    text_prop.encoding = *type;
    text_prop.format = *format;
    text_prop.nitems = *length;

    TRACE(("SelectionReceived %s %s format %d, nitems %ld\n",
	   TraceAtomName(screen->display, *selection),
	   visibleSelectionTarget(dpy, text_prop.encoding),
	   text_prop.format,
	   text_prop.nitems));

#if OPT_WIDE_CHARS
    if (XSupportsLocale() && screen->wide_chars) {
	if (*type == XA_UTF8_STRING(dpy) ||
	    *type == XA_STRING ||
	    *type == XA_COMPOUND_TEXT(dpy)) {
	    GettingSelection(dpy, *type, line, *length);
	    if (Xutf8TextPropertyToTextList(dpy, &text_prop,
					    &text_list,
					    &text_list_count) < 0) {
		TRACE(("default Xutf8 Conversion failed\n"));
		text_list = NULL;
	    }
	}
    } else
#endif /* OPT_WIDE_CHARS */
    {
	/* Convert the selection to locale's multibyte encoding. */

	if (*type == XA_UTF8_STRING(dpy) ||
	    *type == XA_STRING ||
	    *type == XA_COMPOUND_TEXT(dpy)) {
	    Status rc;

	    GettingSelection(dpy, *type, line, *length);

#if OPT_WIDE_CHARS
	    if (*type == XA_UTF8_STRING(dpy) &&
		!(screen->wide_chars || screen->c1_printable)) {
		rc = xtermUtf8ToTextList(xw, &text_prop,
					 &text_list, &text_list_count);
	    } else
#endif
	    if (*type == XA_STRING && (!XSupportsLocale() || screen->brokenSelections)) {
		rc = XTextPropertyToStringList(&text_prop,
					       &text_list, &text_list_count);
	    } else {
		rc = XmbTextPropertyToTextList(dpy, &text_prop,
					       &text_list,
					       &text_list_count);
	    }
	    if (rc < 0) {
		TRACE(("Conversion failed\n"));
		text_list = NULL;
	    }
	}
    }

    if (text_list != NULL && text_list_count != 0) {
	int i;

#if OPT_PASTE64
	if (screen->base64_paste) {
	    /* EMPTY */ ;
	} else
#endif
#if OPT_PASTE64 || OPT_READLINE
	if (SCREEN_FLAG(screen, paste_brackets) && !screen->selectToBuffer) {
	    _WriteKey(screen, (const Char *) "200");
	}
#endif
	for (i = 0; i < text_list_count; i++) {
	    size_t len = removeControls(xw, text_list[i]);

	    if (screen->selectToBuffer) {
		InternalSelect *mydata = &(screen->internal_select);
		if (!mydata->done) {
		    size_t have = (mydata->buffer
				   ? strlen(mydata->buffer)
				   : 0);
		    size_t need = have + len + 1;
		    char *buffer = realloc(mydata->buffer, need);

		    if (buffer != 0) {
			strcpy(buffer + have, text_list[i]);
			mydata->buffer = buffer;
		    }
		    TRACE(("FormatSelect %d.%d .. %d.%d %s\n",
			   screen->startSel.row,
			   screen->startSel.col,
			   screen->endSel.row,
			   screen->endSel.col,
			   mydata->buffer));
		    mydata->format_select(w, mydata->format, mydata->buffer,
					  &(screen->startSel),
					  &(screen->endSel));
		    mydata->done = True;
		}

	    } else {
		_WriteSelectionData(xw, (Char *) text_list[i], len);
	    }
	}
#if OPT_PASTE64
	if (screen->base64_paste) {
	    FinishPaste64(xw);
	} else
#endif
#if OPT_PASTE64 || OPT_READLINE
	if (SCREEN_FLAG(screen, paste_brackets) && !screen->selectToBuffer) {
	    _WriteKey(screen, (const Char *) "201");
	}
#endif
	if (screen->selectToBuffer) {
	    InternalSelect *mydata = &(screen->internal_select);
	    finishInternalSelect(xw);
	    if (mydata->done) {
		free(mydata->format);
		free(mydata->buffer);
		memset(mydata, 0, sizeof(*mydata));
	    }
	    screen->selectToBuffer = False;
	}
	XFreeStringList(text_list);
    } else {
	TRACE(("...empty text-list\n"));
	goto fail;
    }

    XtFree((char *) client_data);
    XtFree((char *) value);

    return;

  fail:
    if (client_data != 0) {
	struct _SelectionList *list = (struct _SelectionList *) client_data;

	TRACE(("SelectionReceived ->xtermGetSelection\n"));
	xtermGetSelection(w, list->time,
			  list->params, list->count, list->targets);
	XtFree((char *) client_data);
#if OPT_PASTE64
    } else {
	FinishPaste64(xw);
#endif
    }
    return;
}