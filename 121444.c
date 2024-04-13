_OwnSelection(XtermWidget xw,
	      String *selections,
	      Cardinal count)
{
    TScreen *screen = TScreenOf(xw);
    Display *dpy = screen->display;
    Atom *atoms = screen->selection_atoms;
    Cardinal i;
    Bool have_selection = False;
    SelectedCells *scp;

    if (count == 0)
	return;

    TRACE(("_OwnSelection count %d\n", count));
    selections = MapSelections(xw, selections, count);

    if (count > screen->sel_atoms_size) {
	XtFree((char *) atoms);
	atoms = TypeXtMallocN(Atom, count);
	screen->selection_atoms = atoms;
	screen->sel_atoms_size = count;
    }
    XmuInternStrings(dpy, selections, count, atoms);
    for (i = 0; i < count; i++) {
	int cutbuffer = CutBuffer(atoms[i]);
	if (cutbuffer >= 0) {
	    unsigned long limit =
	    (unsigned long) (4 * XMaxRequestSize(dpy) - 32);
	    scp = &(screen->selected_cells[CutBufferToCode(cutbuffer)]);
	    if (scp->data_length > limit) {
		TRACE(("selection too big (%lu bytes), not storing in CUT_BUFFER%d\n",
		       scp->data_length, cutbuffer));
		xtermWarning("selection too big (%lu bytes), not storing in CUT_BUFFER%d\n",
			     (unsigned long) scp->data_length, cutbuffer);
	    } else {
		/* This used to just use the UTF-8 data, which was totally
		 * broken as not even the corresponding paste code in xterm
		 * understood this!  So now it converts to Latin1 first.
		 *   Robert Brady, 2000-09-05
		 */
		unsigned long length = scp->data_length;
		Char *data = scp->data_buffer;
		if_OPT_WIDE_CHARS((screen), {
		    data = UTF8toLatin1(screen, data, length, &length);
		});
		TRACE(("XStoreBuffer(%d)\n", cutbuffer));
		XStoreBuffer(dpy,
			     (char *) data,
			     (int) length,
			     cutbuffer);
	    }
	} else {
	    int which = AtomToSelection(dpy, atoms[i]);
	    if (keepClipboard(dpy, atoms[i])) {
		Char *buf;
		SelectedCells *tcp = &(screen->clipboard_data);
		TRACE(("saving selection to clipboard buffer\n"));
		scp = &(screen->selected_cells[CLIPBOARD_CODE]);
		if ((buf = (Char *) malloc((size_t) scp->data_length)) == 0)
		    SysError(ERROR_BMALLOC2);

		free(tcp->data_buffer);
		memcpy(buf, scp->data_buffer, scp->data_length);
		tcp->data_buffer = buf;
		tcp->data_limit = scp->data_length;
		tcp->data_length = scp->data_length;
	    }
	    scp = &(screen->selected_cells[which]);
	    if (scp->data_length == 0) {
		TRACE(("XtDisownSelection(%s, @%ld)\n",
		       TraceAtomName(screen->display, atoms[i]),
		       (long) screen->selection_time));
		XtDisownSelection((Widget) xw,
				  atoms[i],
				  screen->selection_time);
	    } else if (!screen->replyToEmacs && atoms[i] != 0) {
		TRACE(("XtOwnSelection(%s, @%ld)\n",
		       TraceAtomName(screen->display, atoms[i]),
		       (long) screen->selection_time));
		have_selection |=
		    XtOwnSelection((Widget) xw, atoms[i],
				   screen->selection_time,
				   ConvertSelection,
				   LoseSelection,
				   SelectionDone);
	    }
	}
	TRACE(("... _OwnSelection used length %ld value %s\n",
	       scp->data_length,
	       visibleChars(scp->data_buffer,
			    (unsigned) scp->data_length)));
    }
    if (!screen->replyToEmacs)
	screen->selection_count = count;
    if (!have_selection)
	UnHiliteText(xw);
}