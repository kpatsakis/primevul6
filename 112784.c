static int string_tag_find(const char *tag, int len, const char *set) {
  char c, *n;
  const char *t;
  int state=0, done=0;
  char *norm;

  if (len <= 0) {
    return 0;
  }

  norm = (char *)smart_malloc(len+1);

  n = norm;
  t = tag;
  c = tolower(*t);
  /*
    normalize the tag removing leading and trailing whitespace
    and turn any <a whatever...> into just <a> and any </tag>
    into <tag>
  */
  while (!done) {
    switch (c) {
    case '<':
      *(n++) = c;
      break;
    case '>':
      done =1;
      break;
    default:
      if (!isspace((int)c)) {
        if (state == 0) {
          state=1;
        }
        if (c != '/') {
          *(n++) = c;
        }
      } else {
        if (state == 1)
          done=1;
      }
      break;
    }
    c = tolower(*(++t));
  }
  *(n++) = '>';
  *n = '\0';
  if (strstr(set, norm)) {
    done=1;
  } else {
    done=0;
  }
  smart_free(norm);
  return done;
}