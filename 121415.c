_ConvertSelectionHelper(Widget w,
			SelectedCells * scp,
			Atom *type,
			XtPointer *value,
			unsigned long *length,
			int *format,
			int (*conversion_function) (Display *,
						    char **, int,
						    XICCEncodingStyle,
						    XTextProperty *),
			XICCEncodingStyle conversion_style)
{
    *value = 0;
    *length = 0;
    *type = 0;
    *format = 0;

    if (getXtermWidget(w) != 0) {
	Display *dpy = XtDisplay(w);
	XTextProperty textprop;
	int out_n = 0;
	char *result = 0;
	char *the_data = (char *) scp->data_buffer;
	char *the_next;
	unsigned long remaining = scp->data_length;

	TRACE(("converting %ld:'%s'\n",
	       (long) scp->data_length,
	       visibleChars(scp->data_buffer, (unsigned) scp->data_length)));
	/*
	 * For most selections, we can convert in one pass.  It is possible
	 * that some applications contain embedded nulls, e.g., using xterm's
	 * paste64 feature.  For those cases, we will build up the result in
	 * parts.
	 */
	if (memchr(the_data, 0, scp->data_length) != 0) {
	    TRACE(("selection contains embedded nulls\n"));
	    result = calloc(scp->data_length + 1, sizeof(char));
	}

      next_try:
	memset(&textprop, 0, sizeof(textprop));
	if (conversion_function(dpy, &the_data, 1,
				conversion_style,
				&textprop) >= Success) {
	    if ((result != 0)
		&& (textprop.value != 0)
		&& (textprop.format == 8)) {
		char *text_values = (char *) textprop.value;
		unsigned long in_n;

		if (out_n == 0) {
		    *value = result;
		    *type = textprop.encoding;
		    *format = textprop.format;
		}
		for (in_n = 0; in_n < textprop.nitems; ++in_n) {
		    result[out_n++] = text_values[in_n];
		}
		*length += textprop.nitems;
		if ((the_next = memchr(the_data, 0, remaining)) != 0) {
		    unsigned long this_was = (unsigned long) (the_next - the_data);
		    this_was++;
		    the_data += this_was;
		    remaining -= this_was;
		    result[out_n++] = 0;
		    *length += 1;
		    if (remaining)
			goto next_try;
		}
		return True;
	    } else {
		free(result);
		*value = (XtPointer) textprop.value;
		*length = textprop.nitems;
		*type = textprop.encoding;
		*format = textprop.format;
		return True;
	    }
	}
	free(result);
    }
    return False;
}