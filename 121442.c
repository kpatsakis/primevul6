addXtermChar(Char **buffer, Cardinal *used, Cardinal offset, unsigned value)
{
    if (offset + 1 >= *used) {
	*used = 1 + (2 * (offset + 1));
	allocXtermChars(buffer, *used);
    }
    (*buffer)[offset++] = (Char) value;
    return offset;
}