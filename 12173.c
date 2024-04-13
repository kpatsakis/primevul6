qf_parse_line_nomatch(char_u *linebuf, int linelen, qffields_T *fields)
{
    fields->namebuf[0] = NUL;	// no match found, remove file name
    fields->lnum = 0;		// don't jump to this line
    fields->valid = FALSE;

    return copy_nonerror_line(linebuf, linelen, fields);
}