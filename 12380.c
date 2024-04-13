nv_object(
    cmdarg_T	*cap)
{
    int		flag;
    int		include;
    char_u	*mps_save;

    if (cap->cmdchar == 'i')
	include = FALSE;    // "ix" = inner object: exclude white space
    else
	include = TRUE;	    // "ax" = an object: include white space

    // Make sure (), [], {} and <> are in 'matchpairs'
    mps_save = curbuf->b_p_mps;
    curbuf->b_p_mps = (char_u *)"(:),{:},[:],<:>";

    switch (cap->nchar)
    {
	case 'w': // "aw" = a word
		flag = current_word(cap->oap, cap->count1, include, FALSE);
		break;
	case 'W': // "aW" = a WORD
		flag = current_word(cap->oap, cap->count1, include, TRUE);
		break;
	case 'b': // "ab" = a braces block
	case '(':
	case ')':
		flag = current_block(cap->oap, cap->count1, include, '(', ')');
		break;
	case 'B': // "aB" = a Brackets block
	case '{':
	case '}':
		flag = current_block(cap->oap, cap->count1, include, '{', '}');
		break;
	case '[': // "a[" = a [] block
	case ']':
		flag = current_block(cap->oap, cap->count1, include, '[', ']');
		break;
	case '<': // "a<" = a <> block
	case '>':
		flag = current_block(cap->oap, cap->count1, include, '<', '>');
		break;
	case 't': // "at" = a tag block (xml and html)
		// Do not adjust oap->end in do_pending_operator()
		// otherwise there are different results for 'dit'
		// (note leading whitespace in last line):
		// 1) <b>      2) <b>
		//    foobar      foobar
		//    </b>            </b>
		cap->retval |= CA_NO_ADJ_OP_END;
		flag = current_tagblock(cap->oap, cap->count1, include);
		break;
	case 'p': // "ap" = a paragraph
		flag = current_par(cap->oap, cap->count1, include, 'p');
		break;
	case 's': // "as" = a sentence
		flag = current_sent(cap->oap, cap->count1, include);
		break;
	case '"': // "a"" = a double quoted string
	case '\'': // "a'" = a single quoted string
	case '`': // "a`" = a backtick quoted string
		flag = current_quote(cap->oap, cap->count1, include,
								  cap->nchar);
		break;
#if 0	// TODO
	case 'S': // "aS" = a section
	case 'f': // "af" = a filename
	case 'u': // "au" = a URL
#endif
	default:
		flag = FAIL;
		break;
    }

    curbuf->b_p_mps = mps_save;
    if (flag == FAIL)
	clearopbeep(cap->oap);
    adjust_cursor_col();
    curwin->w_set_curswant = TRUE;
}