_nc_mouse_parse(SCREEN *sp, int runcount)
/* parse a run of atomic mouse events into a gesture */
{
    MEVENT *eventp = sp->_mouse_eventp;
    MEVENT *next, *ep;
    MEVENT *first_valid = NULL;
    MEVENT *first_invalid = NULL;
    int n;
    int b;
    bool merge;
    bool endLoop;

    TR(MY_TRACE, ("_nc_mouse_parse(%d) called", runcount));

    /*
     * When we enter this routine, the event list next-free pointer
     * points just past a run of mouse events that we know were separated
     * in time by less than the critical click interval. The job of this
     * routine is to collapse this run into a single higher-level event
     * or gesture.
     *
     * We accomplish this in two passes.  The first pass merges press/release
     * pairs into click events.  The second merges runs of click events into
     * double or triple-click events.
     *
     * It's possible that the run may not resolve to a single event (for
     * example, if the user quadruple-clicks).  If so, leading events
     * in the run are ignored if user does not call getmouse in a loop (getting
     * them from newest to older).
     *
     * Note that this routine is independent of the format of the specific
     * format of the pointing-device's reports.  We can use it to parse
     * gestures on anything that reports press/release events on a per-
     * button basis, as long as the device-dependent mouse code puts stuff
     * on the queue in MEVENT format.
     */

    /*
     * Reset all events that were not set, in case the user sometimes calls
     * getmouse only once and other times until there are no more events in
     * queue.
     *
     * This also allows reaching the beginning of the run.
     */
    ep = eventp;
    for (n = runcount; n < EV_MAX; n++) {
	Invalidate(ep);
	ep = NEXT(ep);
    }

#ifdef TRACE
    if (USE_TRACEF(TRACE_IEVENT)) {
	_trace_slot(sp, "before mouse press/release merge:");
	_tracef("_nc_mouse_parse: run starts at %ld, ends at %ld, count %d",
		RunParams(sp, eventp, ep),
		runcount);
	_nc_unlock_global(tracef);
    }
#endif /* TRACE */

    /* first pass; merge press/release pairs */
    endLoop = FALSE;
    while (!endLoop) {
	next = NEXT(ep);
	if (next == eventp) {
	    /* Will end the loop, but compact before */
	    endLoop = TRUE;
	} else {

#define MASK_CHANGED(x) (!(ep->bstate & MASK_PRESS(x)) \
		      == !(next->bstate & MASK_RELEASE(x)))

	    if (ValidEvent(ep) && ValidEvent(next)
		&& ep->x == next->x && ep->y == next->y
		&& (ep->bstate & BUTTON_PRESSED)
		&& (!(next->bstate & BUTTON_PRESSED))) {
		bool changed = TRUE;

		for (b = 1; b <= MAX_BUTTONS; ++b) {
		    if (!MASK_CHANGED(b)) {
			changed = FALSE;
			break;
		    }
		}

		if (changed) {
		    merge = FALSE;
		    for (b = 1; b <= MAX_BUTTONS; ++b) {
			if ((sp->_mouse_mask & MASK_CLICK(b))
			    && (ep->bstate & MASK_PRESS(b))) {
			    next->bstate &= ~MASK_RELEASE(b);
			    next->bstate |= MASK_CLICK(b);
			    merge = TRUE;
			}
		    }
		    if (merge) {
			Invalidate(ep);
		    }
		}
	    }
	}

	/* Compact valid events */
	if (!ValidEvent(ep)) {
	    if ((first_valid != NULL) && (first_invalid == NULL)) {
		first_invalid = ep;
	    }
	} else {
	    if (first_valid == NULL) {
		first_valid = ep;
	    } else if (first_invalid != NULL) {
		*first_invalid = *ep;
		Invalidate(ep);
		first_invalid = NEXT(first_invalid);
	    }
	}

	ep = next;
    }

    if (first_invalid != NULL) {
	eventp = first_invalid;
    }
#ifdef TRACE
    if (USE_TRACEF(TRACE_IEVENT)) {
	_trace_slot(sp, "before mouse click merge:");
	if (first_valid == NULL) {
	    _tracef("_nc_mouse_parse: no valid event");
	} else {
	    _tracef("_nc_mouse_parse: run starts at %ld, ends at %ld, count %d",
		    RunParams(sp, eventp, first_valid),
		    runcount);
	    _nc_unlock_global(tracef);
	}
    }
#endif /* TRACE */

    /*
     * Second pass; merge click runs.  We merge click events forward in the
     * queue.  For example, double click can be changed to triple click.
     *
     * NOTE: There is a problem with this design!  If the application
     * allows enough click events to pile up in the circular queue so
     * they wrap around, it will cheerfully merge the newest forward
     * into the oldest, creating a bogus doubleclick and confusing
     * the queue-traversal logic rather badly.  Generally this won't
     * happen, because calling getmouse() marks old events invalid and
     * ineligible for merges.  The true solution to this problem would
     * be to timestamp each MEVENT and perform the obvious sanity check,
     * but the timer element would have to have sub-second resolution,
     * which would get us into portability trouble.
     */
    first_invalid = NULL;
    endLoop = (first_valid == NULL);
    ep = first_valid;
    while (!endLoop) {
	next = NEXT(ep);

	if (next == eventp) {
	    /* Will end the loop, but check event type and compact before */
	    endLoop = TRUE;
	} else if (!ValidEvent(next)) {
	    continue;
	} else {
	    /* merge click events forward */
	    if ((ep->bstate & BUTTON_CLICKED)
		&& (next->bstate & BUTTON_CLICKED)) {
		merge = FALSE;
		for (b = 1; b <= MAX_BUTTONS; ++b) {
		    if ((sp->_mouse_mask & MASK_DOUBLE_CLICK(b))
			&& (ep->bstate & MASK_CLICK(b))
			&& (next->bstate & MASK_CLICK(b))) {
			next->bstate &= ~MASK_CLICK(b);
			next->bstate |= MASK_DOUBLE_CLICK(b);
			merge = TRUE;
		    }
		}
		if (merge) {
		    Invalidate(ep);
		}
	    }

	    /* merge double-click events forward */
	    if ((ep->bstate & BUTTON_DOUBLE_CLICKED)
		&& (next->bstate & BUTTON_CLICKED)) {
		merge = FALSE;
		for (b = 1; b <= MAX_BUTTONS; ++b) {
		    if ((sp->_mouse_mask & MASK_TRIPLE_CLICK(b))
			&& (ep->bstate & MASK_DOUBLE_CLICK(b))
			&& (next->bstate & MASK_CLICK(b))) {
			next->bstate &= ~MASK_CLICK(b);
			next->bstate |= MASK_TRIPLE_CLICK(b);
			merge = TRUE;
		    }
		}
		if (merge) {
		    Invalidate(ep);
		}
	    }
	}

	/* Discard event if it does not match event mask */
	if (!(ep->bstate & sp->_mouse_mask2)) {
	    Invalidate(ep);
	}

	/* Compact valid events */
	if (!ValidEvent(ep)) {
	    if (ep == first_valid) {
		first_valid = next;
	    } else if (first_invalid == NULL) {
		first_invalid = ep;
	    }
	} else if (first_invalid != NULL) {
	    *first_invalid = *ep;
	    Invalidate(ep);
	    first_invalid = NEXT(first_invalid);
	}

	ep = next;
    }

    if (first_invalid == NULL) {
	first_invalid = eventp;
    }
    sp->_mouse_eventp = first_invalid;

#ifdef TRACE
    if (first_valid != NULL) {
	if (USE_TRACEF(TRACE_IEVENT)) {
	    _trace_slot(sp, "after mouse event queue compaction:");
	    _tracef("_nc_mouse_parse: run starts at %ld, ends at %ld, count %d",
		    RunParams(sp, first_invalid, first_valid),
		    runcount);
	    _nc_unlock_global(tracef);
	}
	for (ep = first_valid; ep != first_invalid; ep = NEXT(ep)) {
	    if (ValidEvent(ep))
		TR(MY_TRACE,
		   ("_nc_mouse_parse: returning composite mouse event %s at slot %ld",
		    _nc_tracemouse(sp, ep),
		    (long) IndexEV(sp, ep)));
	}
    }
#endif /* TRACE */

    /* after all this, do we have a valid event? */
    return ValidEvent(PREV(first_invalid));
}