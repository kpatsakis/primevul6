String string_soundex(const String& str) {
  assertx(!str.empty());
  int _small, code, last;
  String retString(4, ReserveString);
  char* soundex = retString.mutableData();

  static char soundex_table[26] = {
    0,              /* A */
    '1',            /* B */
    '2',            /* C */
    '3',            /* D */
    0,              /* E */
    '1',            /* F */
    '2',            /* G */
    0,              /* H */
    0,              /* I */
    '2',            /* J */
    '2',            /* K */
    '4',            /* L */
    '5',            /* M */
    '5',            /* N */
    0,              /* O */
    '1',            /* P */
    '2',            /* Q */
    '6',            /* R */
    '2',            /* S */
    '3',            /* T */
    0,              /* U */
    '1',            /* V */
    0,              /* W */
    '2',            /* X */
    0,              /* Y */
    '2'             /* Z */
  };

  /* build soundex string */
  last = -1;
  auto p = str.slice().data();
  for (_small = 0; *p && _small < 4; p++) {
    /* convert chars to upper case and strip non-letter chars */
    /* BUG: should also map here accented letters used in non */
    /* English words or names (also found in English text!): */
    /* esstsett, thorn, n-tilde, c-cedilla, s-caron, ... */
    code = toupper((int)(unsigned char)(*p));
    if (code >= 'A' && code <= 'Z') {
      if (_small == 0) {
        /* remember first valid char */
        soundex[_small++] = code;
        last = soundex_table[code - 'A'];
      } else {
        /* ignore sequences of consonants with same soundex */
        /* code in trail, and vowels unless they separate */
        /* consonant letters */
        code = soundex_table[code - 'A'];
        if (code != last) {
          if (code != 0) {
            soundex[_small++] = code;
          }
          last = code;
        }
      }
    }
  }
  /* pad with '0' and terminate with 0 ;-) */
  while (_small < 4) {
    soundex[_small++] = '0';
  }
  retString.setSize(4);
  return retString;
}