GettingSelection(Display *dpy, Atom type, Char *line, unsigned long len)
{
    Char *cp;
    const char *name = TraceAtomName(dpy, type);

    TRACE(("Getting %s (type=%ld, length=%ld)\n", name, (long int) type, len));
    for (cp = line; cp < line + len; cp++) {
	TRACE(("[%d:%lu]", (int) (cp + 1 - line), len));
	if (isprint(*cp)) {
	    TRACE(("%c\n", *cp));
	} else {
	    TRACE(("\\x%02x\n", *cp));
	}
    }
}