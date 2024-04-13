static char* xml_elem_entity_escape(const char* buf, int old_len, int *newlen, XML_ELEM_ESCAPING flags) {
  char *pRetval = 0;
  int iNewBufLen=0;

#define should_escape(c, flag) ( ((flag & xml_elem_markup_escaping) && markup(c)) || \
                                 ((flag & xml_elem_non_ascii_escaping) && non_ascii(c)) || \
                                 ((flag & xml_elem_non_print_escaping) && non_print(c)) )

  if(buf && *buf) {
    const unsigned char *bufcopy;
    char *NewBuffer;
    int ToBeXmlEscaped=0;
    int iLength;
    bufcopy = buf;
    iLength= old_len ? old_len : strlen(buf);
    while(*bufcopy) {
      if( should_escape(*bufcopy, flags) ) {
	/* the length will increase by length of xml escape - the character length */
	iLength += entity_length(*bufcopy);
	ToBeXmlEscaped=1;
      }
      bufcopy++;
    }

    if(ToBeXmlEscaped) {

      NewBuffer= malloc(iLength+1);
      if(NewBuffer) {
	bufcopy=buf;
	while(*bufcopy) {
	  if(should_escape(*bufcopy, flags)) {
	    iNewBufLen += create_xml_escape(NewBuffer+iNewBufLen,*bufcopy);
	  }
	  else {
	    NewBuffer[iNewBufLen++]=*bufcopy;
	  }
	  bufcopy++;
	}
	NewBuffer[iNewBufLen] = 0;
	pRetval = NewBuffer;
      }
    }
  }

  if(newlen) {
     *newlen = iNewBufLen;
  }

  return pRetval;
}