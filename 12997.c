drill_file_p(gerb_file_t *fd, gboolean *returnFoundBinary)
{
    char *buf=NULL, *tbuf;
    int len = 0;
    char *letter;
    int ascii;
    int zero = 48; /* ascii 0 */
    int nine = 57; /* ascii 9 */
    int i;
    gboolean found_binary = FALSE;
    gboolean found_M48 = FALSE;
    gboolean found_M30 = FALSE;
    gboolean found_percent = FALSE;
    gboolean found_T = FALSE;
    gboolean found_X = FALSE;
    gboolean found_Y = FALSE;
    gboolean end_comments=FALSE;
 
    tbuf = g_malloc(MAXL);
    if (tbuf == NULL) 
	GERB_FATAL_ERROR(
		"malloc buf failed while checking for drill file in %s()",
		__FUNCTION__);

    while (fgets(tbuf, MAXL, fd->fd) != NULL) {
	len = strlen(tbuf);
	buf = tbuf;
	/* check for comments at top of file.  */
	if(!end_comments){
		if(g_strstr_len(buf, len, ";")!=NULL){/* comments at top of file  */
			for (i = 0; i < len-1; ++i) {
				if (buf[i] == '\n'
				&&  buf[i+1] != ';'
				&&  buf[i+1] != '\r'
				&&  buf[i+1] != '\n') {
					end_comments = TRUE;
					/* Set rest of parser to end of
					 * comments */
					buf = &tbuf[i+1];
					
				}
			}
			if(!end_comments)
				continue;
		}			
		else 
			end_comments=TRUE;
	}

	/* First look through the file for indications of its type */
	len = strlen(buf);
	/* check that file is not binary (non-printing chars) */
	for (i = 0; i < len; i++) {
	    ascii = (int) buf[i];
	    if ((ascii > 128) || (ascii < 0)) {
		found_binary = TRUE;
	    }
	}

	/* Check for M48 = start of drill header */
	if (g_strstr_len(buf, len, "M48")) {
	    found_M48 = TRUE; 
	}

	/* Check for M30 = end of drill program */
	if (g_strstr_len(buf, len, "M30")) {
	    if (found_percent) {
		found_M30 = TRUE; /* Found M30 after % = good */
	    }
	}

	/* Check for % on its own line at end of header */
	if ((letter = g_strstr_len(buf, len, "%")) != NULL) {
	    if ((letter[1] ==  '\r') || (letter[1] ==  '\n'))
		found_percent = TRUE;
	}

	/* Check for T<number> */
	if ((letter = g_strstr_len(buf, len, "T")) != NULL) {
	    if (!found_T && (found_X || found_Y)) {
		found_T = FALSE;  /* Found first T after X or Y */
	    } else {
		if (isdigit( (int) letter[1])) { /* verify next char is digit */
		    found_T = TRUE;
		}
	    }
	}

	/* look for X<number> or Y<number> */
	if ((letter = g_strstr_len(buf, len, "X")) != NULL) {
	    ascii = (int) letter[1]; /* grab char after X */
	    if ((ascii >= zero) && (ascii <= nine)) {
		found_X = TRUE;
	    }
	}
	if ((letter = g_strstr_len(buf, len, "Y")) != NULL) {
	    ascii = (int) letter[1]; /* grab char after Y */
	    if ((ascii >= zero) && (ascii <= nine)) {
		found_Y = TRUE;
	    }
	}
    } /* while (fgets(buf, MAXL, fd->fd) */

    rewind(fd->fd);
    g_free(tbuf);
    *returnFoundBinary = found_binary;
    
    /* Now form logical expression determining if this is a drill file */
    if ( ((found_X || found_Y) && found_T) && 
	 (found_M48 || (found_percent && found_M30)) ) 
	return TRUE;
    else if (found_M48 && found_T && found_percent && found_M30)
	/* Pathological case of drill file with valid header 
	   and EOF but no drill XY locations. */
	return TRUE;
    else 
	return FALSE;
} /* drill_file_p */