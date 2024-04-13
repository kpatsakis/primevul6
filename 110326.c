xmlParseNCNameComplex(xmlParserCtxtPtr ctxt) {
    int len = 0, l;
    int c;
    int count = 0;
    const xmlChar *end; /* needed because CUR_CHAR() can move cur on \r\n */

#ifdef DEBUG
    nbParseNCNameComplex++;
#endif

    /*
     * Handler for more complex cases
     */
    GROW;
    end = ctxt->input->cur;
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || /* accelerators */
	(!xmlIsNameStartChar(ctxt, c) || (c == ':'))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && /* test bigname.xml */
	   (xmlIsNameChar(ctxt, c) && (c != ':'))) {
	if (count++ > XML_PARSER_CHUNK_SIZE) {
            if ((len > XML_MAX_NAME_LENGTH) &&
                ((ctxt->options & XML_PARSE_HUGE) == 0)) {
                xmlFatalErr(ctxt, XML_ERR_NAME_TOO_LONG, "NCName");
                return(NULL);
            }
	    count = 0;
	    GROW;
            if (ctxt->instate == XML_PARSER_EOF)
                return(NULL);
	}
	len += l;
	NEXTL(l);
	end = ctxt->input->cur;
	c = CUR_CHAR(l);
	if (c == 0) {
	    count = 0;
	    /*
	     * when shrinking to extend the buffer we really need to preserve
	     * the part of the name we already parsed. Hence rolling back
	     * by current lenght.
	     */
	    ctxt->input->cur -= l;
	    GROW;
	    ctxt->input->cur += l;
            if (ctxt->instate == XML_PARSER_EOF)
                return(NULL);
	    end = ctxt->input->cur;
	    c = CUR_CHAR(l);
	}
    }
    if ((len > XML_MAX_NAME_LENGTH) &&
        ((ctxt->options & XML_PARSE_HUGE) == 0)) {
        xmlFatalErr(ctxt, XML_ERR_NAME_TOO_LONG, "NCName");
        return(NULL);
    }
    return(xmlDictLookup(ctxt->dict, end - len, len));
}