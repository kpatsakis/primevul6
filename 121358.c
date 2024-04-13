xtermGetSelection(Widget w,
		  Time ev_time,
		  String *params,	/* selections in precedence order */
		  Cardinal num_params,
		  Atom *targets)
{
    Atom selection;
    int cutbuffer;
    Atom target;

    XtermWidget xw;

    if (num_params == 0)
	return;
    if ((xw = getXtermWidget(w)) == 0)
	return;

    TRACE(("xtermGetSelection num_params %d @%ld\n", num_params, ev_time));
    params = MapSelections(xw, params, num_params);

    XmuInternStrings(XtDisplay(w), params, (Cardinal) 1, &selection);
    cutbuffer = CutBuffer(selection);

    TRACE(("Cutbuffer: %d, target: %s\n", cutbuffer,
	   (targets
	    ? visibleSelectionTarget(XtDisplay(w), targets[0])
	    : "None")));

    if (cutbuffer >= 0) {
	int inbytes;
	unsigned long nbytes;
	int fmt8 = 8;
	Atom type = XA_STRING;
	char *line;

	/* 'line' is freed in SelectionReceived */
	line = XFetchBuffer(XtDisplay(w), &inbytes, cutbuffer);
	nbytes = (unsigned long) inbytes;

	if (nbytes > 0) {
	    SelectionReceived(w, NULL, &selection, &type, (XtPointer) line,
			      &nbytes, &fmt8);
	} else if (num_params > 1) {
	    xtermGetSelection(w, ev_time, params + 1, num_params - 1, NULL);
	}
#if OPT_PASTE64
	else {
	    FinishPaste64(xw);
	}
#endif
    } else {

	if (targets == NULL || targets[0] == None) {
	    targets = _SelectionTargets(w);
	}

	if (targets != 0) {
	    struct _SelectionList *list;

	    target = targets[0];

	    if (targets[1] == None) {	/* last target in list */
		params++;
		num_params--;
		targets = _SelectionTargets(w);
	    } else {
		targets = &(targets[1]);
	    }

	    if (num_params) {
		/* 'list' is freed in SelectionReceived */
		list = TypeXtMalloc(struct _SelectionList);
		if (list != 0) {
		    list->params = params;
		    list->count = num_params;
		    list->targets = targets;
		    list->time = ev_time;
		}
	    } else {
		list = NULL;
	    }

	    XtGetSelectionValue(w, selection,
				target,
				SelectionReceived,
				(XtPointer) list, ev_time);
	}
    }
}