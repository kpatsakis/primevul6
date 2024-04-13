TargetToSelection(TScreen *screen, String name)
{
    int result = -1;
    int cutb;

    if (isSELECT(name)) {
	result = DefaultSelection(screen);
    } else if (!strcmp(name, PRIMARY_NAME)) {
	result = PRIMARY_CODE;
    } else if (!strcmp(name, CLIPBOARD_NAME)) {
	result = CLIPBOARD_CODE;
    } else if (!strcmp(name, SECONDARY_NAME)) {
	result = SECONDARY_CODE;
    } else if (sscanf(name, "CUT_BUFFER%d", &cutb) == 1) {
	if (cutb >= 0 && cutb < MAX_CUT_BUFFER) {
	    result = CutBufferToCode(cutb);
	} else {
	    xtermWarning("unexpected cut-buffer code: %d\n", cutb);
	}
    } else {
	xtermWarning("unexpected selection target: %s\n", name);
    }
    TRACE2(("TargetToSelection(%s) ->%d\n", name, result));
    return result;
}