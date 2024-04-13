static int ac_match_handler(AC_MATCH_t *m, AC_TEXT_t *txt, AC_REP_t *match) {
  int min_len = (txt->length < m->patterns->length) ? txt->length : m->patterns->length;
  char buf[64] = {'\0'}, *whatfound;
  int min_buf_len = (txt->length > 63 /* sizeof(buf)-1 */) ? 63 : txt->length;
  u_int buf_len = strlen(buf);

  strncpy(buf, txt->astring, min_buf_len);
  buf[min_buf_len] = '\0';

#ifdef MATCH_DEBUG
  printf("Searching [to search: %s/%u][pattern: %s/%u] [len: %d][match_num: %u][%s]\n", buf,
	 (unigned int) txt->length, m->patterns->astring, (unigned int) m->patterns->length, min_len, m->match_num,
	 m->patterns->astring);
#endif

  whatfound = strstr(buf, m->patterns->astring);

#ifdef MATCH_DEBUG
  printf("[NDPI] %s() [searching=%s][pattern=%s][%s][%c]\n", __FUNCTION__, buf, m->patterns->astring,
	 whatfound ? whatfound : "<NULL>", whatfound[-1]);
#endif

  if(whatfound) {
    /*
      The patch below allows in case of pattern ws.amazon.com
      to avoid matching aws.amazon.com whereas a.ws.amazon.com
      has to match
    */
    if((whatfound != buf) && (m->patterns->astring[0] != '.') /* The searched pattern does not start with . */
       && strchr(m->patterns->astring, '.') /* The matched pattern has a . (e.g. numeric or sym IPs) */) {
      int len = strlen(m->patterns->astring);

      if((whatfound[-1] != '.') || ((m->patterns->astring[len - 1] != '.') &&
				    (whatfound[len] != '\0') /* endsWith does not hold here */)) {
	return(0);
      } else {
	memcpy(match, &m->patterns[0].rep, sizeof(AC_REP_t)); /* Partial match? */
	return(0); /* Keep searching as probably there is a better match */
      }
    }
  }

  /*
    Return 1 for stopping to the first match.
    We might consider searching for the more
    specific match, paying more cpu cycles.
  */
  memcpy(match, &m->patterns[0].rep, sizeof(AC_REP_t));

  if(((buf_len >= min_len) && (strncmp(&buf[buf_len - min_len], m->patterns->astring, min_len) == 0)) ||
     (strncmp(buf, m->patterns->astring, min_len) == 0) /* begins with */
     ) {
#ifdef MATCH_DEBUG
    printf("Found match [%s][%s] [len: %d]"
	   // "[proto_id: %u]"
	   "\n",
	   buf, m->patterns->astring, min_len /* , *matching_protocol_id */);
#endif
    return(1); /* If the pattern found matches the string at the beginning we stop here */
  } else {
#ifdef MATCH_DEBUG
    printf("NO match found: continue\n");
#endif
    return(0); /* 0 to continue searching, !0 to stop */
  }
}