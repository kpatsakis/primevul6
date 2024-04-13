MapSelections(XtermWidget xw, String *params, Cardinal num_params)
{
    String *result = params;

    if (params != 0 && num_params > 0) {
	Cardinal j;
	Boolean map = False;

	for (j = 0; j < num_params; ++j) {
	    TRACE(("param[%d]:%s\n", j, params[j]));
	    if (isSELECT(params[j])) {
		map = True;
		break;
	    }
	}
	if (map) {
	    TScreen *screen = TScreenOf(xw);
	    const char *mapTo = (screen->selectToClipboard
				 ? CLIPBOARD_NAME
				 : PRIMARY_NAME);

	    UnmapSelections(xw);
	    if ((result = TypeMallocN(String, num_params + 1)) != 0) {
		result[num_params] = 0;
		for (j = 0; j < num_params; ++j) {
		    result[j] = x_strdup((isSELECT(params[j])
					  ? mapTo
					  : params[j]));
		    if (result[j] == 0) {
			UnmapSelections(xw);
			while (j != 0) {
			    free((void *) result[--j]);
			}
			FreeAndNull(result);
			break;
		    }
		}
		screen->mappedSelect = result;
	    }
	}
    }
    return result;
}