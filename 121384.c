SaveConvertedLength(XtPointer *target, unsigned long source)
{
    Boolean result = False;

    *target = XtMalloc(4);
    if (*target != 0) {
	result = True;
	if (sizeof(unsigned long) == 4) {
	    *(unsigned long *) *target = source;
	} else if (sizeof(unsigned) == 4) {
	    *(unsigned *) *target = (unsigned) source;
	} else if (sizeof(unsigned short) == 4) {
	    *(unsigned short *) *target = (unsigned short) source;
	} else {
	    /* FIXME - does this depend on byte-order? */
	    unsigned long temp = source;
	    memcpy((char *) *target,
		   ((char *) &temp) + sizeof(temp) - 4,
		   (size_t) 4);
	}
    }
    return result;
}