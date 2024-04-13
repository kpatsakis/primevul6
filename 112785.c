String string_strip_tags(const char *s, const int len,
                         const char *allow, const int allow_len,
                         bool allow_tag_spaces) {
  const char *abuf, *p;
  char *rbuf, *tbuf, *tp, *rp, c, lc;

  int br, i=0, depth=0, in_q = 0;
  int state = 0, pos;

  assert(s);
  assert(allow);

  String retString(s, len, CopyString);
  rbuf = retString.bufferSlice().ptr;
  String allowString;

  c = *s;
  lc = '\0';
  p = s;
  rp = rbuf;
  br = 0;
  if (allow_len) {
    assert(allow);

    allowString = String(allow_len, ReserveString);
    char *atmp = allowString.bufferSlice().ptr;
    for (const char *tmp = allow; *tmp; tmp++, atmp++) {
      *atmp = tolower((int)*(const unsigned char *)tmp);
    }
    allowString.setSize(allow_len);
    abuf = allowString.data();

    tbuf = (char *)smart_malloc(PHP_TAG_BUF_SIZE+1);
    tp = tbuf;
  } else {
    abuf = nullptr;
    tbuf = tp = nullptr;
  }

  auto move = [&pos, &tbuf, &tp]() {
    if (tp - tbuf >= PHP_TAG_BUF_SIZE) {
      pos = tp - tbuf;
      tbuf = (char*)smart_realloc(tbuf, (tp - tbuf) + PHP_TAG_BUF_SIZE + 1);
      tp = tbuf + pos;
    }
  };

  while (i < len) {
    switch (c) {
    case '\0':
      break;
    case '<':
      if (isspace(*(p + 1)) && !allow_tag_spaces) {
        goto reg_char;
      }
      if (state == 0) {
        lc = '<';
        state = 1;
        if (allow_len) {
          move();
          *(tp++) = '<';
        }
      } else if (state == 1) {
        depth++;
      }
      break;

    case '(':
      if (state == 2) {
        if (lc != '"' && lc != '\'') {
          lc = '(';
          br++;
        }
      } else if (allow_len && state == 1) {
        move();
        *(tp++) = c;
      } else if (state == 0) {
        *(rp++) = c;
      }
      break;

    case ')':
      if (state == 2) {
        if (lc != '"' && lc != '\'') {
          lc = ')';
          br--;
        }
      } else if (allow_len && state == 1) {
        move();
        *(tp++) = c;
      } else if (state == 0) {
        *(rp++) = c;
      }
      break;

    case '>':
      if (depth) {
        depth--;
        break;
      }

      if (in_q) {
        break;
      }

      switch (state) {
      case 1: /* HTML/XML */
        lc = '>';
        in_q = state = 0;
        if (allow_len) {
          move();
          *(tp++) = '>';
          *tp='\0';
          if (string_tag_find(tbuf, tp-tbuf, abuf)) {
            memcpy(rp, tbuf, tp-tbuf);
            rp += tp-tbuf;
          }
          tp = tbuf;
        }
        break;

      case 2: /* PHP */
        if (!br && lc != '\"' && *(p-1) == '?') {
          in_q = state = 0;
          tp = tbuf;
        }
        break;

      case 3:
        in_q = state = 0;
        tp = tbuf;
        break;

      case 4: /* JavaScript/CSS/etc... */
        if (p >= s + 2 && *(p-1) == '-' && *(p-2) == '-') {
          in_q = state = 0;
          tp = tbuf;
        }
        break;

      default:
        *(rp++) = c;
        break;
      }
      break;

    case '"':
    case '\'':
      if (state == 4) {
        /* Inside <!-- comment --> */
        break;
      } else if (state == 2 && *(p-1) != '\\') {
        if (lc == c) {
          lc = '\0';
        } else if (lc != '\\') {
          lc = c;
        }
      } else if (state == 0) {
        *(rp++) = c;
      } else if (allow_len && state == 1) {
        move();
        *(tp++) = c;
      }
      if (state && p != s && *(p-1) != '\\' && (!in_q || *p == in_q)) {
        if (in_q) {
          in_q = 0;
        } else {
          in_q = *p;
        }
      }
      break;

    case '!':
      /* JavaScript & Other HTML scripting languages */
      if (state == 1 && *(p-1) == '<') {
        state = 3;
        lc = c;
      } else {
        if (state == 0) {
          *(rp++) = c;
        } else if (allow_len && state == 1) {
          move();
          *(tp++) = c;
        }
      }
      break;

    case '-':
      if (state == 3 && p >= s + 2 && *(p-1) == '-' && *(p-2) == '!') {
        state = 4;
      } else {
        goto reg_char;
      }
      break;

    case '?':

      if (state == 1 && *(p-1) == '<') {
        br=0;
        state=2;
        break;
      }

    case 'E':
    case 'e':
      /* !DOCTYPE exception */
      if (state==3 && p > s+6
          && tolower(*(p-1)) == 'p'
          && tolower(*(p-2)) == 'y'
          && tolower(*(p-3)) == 't'
          && tolower(*(p-4)) == 'c'
          && tolower(*(p-5)) == 'o'
          && tolower(*(p-6)) == 'd') {
        state = 1;
        break;
      }
      /* fall-through */

    case 'l':

      /* swm: If we encounter '<?xml' then we shouldn't be in
       * state == 2 (PHP). Switch back to HTML.
       */

      if (state == 2 && p > s+2 && *(p-1) == 'm' && *(p-2) == 'x') {
        state = 1;
        break;
      }

      /* fall-through */
    default:
    reg_char:
      if (state == 0) {
        *(rp++) = c;
      } else if (allow_len && state == 1) {
        move();
        *(tp++) = c;
      }
      break;
    }
    c = *(++p);
    i++;
  }
  if (rp < rbuf + len) {
    *rp = '\0';
  }
  if (allow_len) {
    smart_free(tbuf);
  }

  retString.setSize(rp - rbuf);
  return retString;
}