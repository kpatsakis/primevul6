report_char_class(XtermWidget xw)
{
    /* simple table, to match documentation */
    static const char charnames[] =
    "NUL\0" "SOH\0" "STX\0" "ETX\0" "EOT\0" "ENQ\0" "ACK\0" "BEL\0"
    " BS\0" " HT\0" " NL\0" " VT\0" " NP\0" " CR\0" " SO\0" " SI\0"
    "DLE\0" "DC1\0" "DC2\0" "DC3\0" "DC4\0" "NAK\0" "SYN\0" "ETB\0"
    "CAN\0" " EM\0" "SUB\0" "ESC\0" " FS\0" " GS\0" " RS\0" " US\0"
    " SP\0" "  !\0" "  \"\0" "  #\0" "  $\0" "  %\0" "  &\0" "  '\0"
    "  (\0" "  )\0" "  *\0" "  +\0" "  ,\0" "  -\0" "  .\0" "  /\0"
    "  0\0" "  1\0" "  2\0" "  3\0" "  4\0" "  5\0" "  6\0" "  7\0"
    "  8\0" "  9\0" "  :\0" "  ;\0" "  <\0" "  =\0" "  >\0" "  ?\0"
    "  @\0" "  A\0" "  B\0" "  C\0" "  D\0" "  E\0" "  F\0" "  G\0"
    "  H\0" "  I\0" "  J\0" "  K\0" "  L\0" "  M\0" "  N\0" "  O\0"
    "  P\0" "  Q\0" "  R\0" "  S\0" "  T\0" "  U\0" "  V\0" "  W\0"
    "  X\0" "  Y\0" "  Z\0" "  [\0" "  \\\0" "  ]\0" "  ^\0" "  _\0"
    "  `\0" "  a\0" "  b\0" "  c\0" "  d\0" "  e\0" "  f\0" "  g\0"
    "  h\0" "  i\0" "  j\0" "  k\0" "  l\0" "  m\0" "  n\0" "  o\0"
    "  p\0" "  q\0" "  r\0" "  s\0" "  t\0" "  u\0" "  v\0" "  w\0"
    "  x\0" "  y\0" "  z\0" "  {\0" "  |\0" "  }\0" "  ~\0" "DEL\0"
    "x80\0" "x81\0" "x82\0" "x83\0" "IND\0" "NEL\0" "SSA\0" "ESA\0"
    "HTS\0" "HTJ\0" "VTS\0" "PLD\0" "PLU\0" " RI\0" "SS2\0" "SS3\0"
    "DCS\0" "PU1\0" "PU2\0" "STS\0" "CCH\0" " MW\0" "SPA\0" "EPA\0"
    "x98\0" "x99\0" "x9A\0" "CSI\0" " ST\0" "OSC\0" " PM\0" "APC\0"
    "  -\0" "  i\0" " c/\0" "  L\0" " ox\0" " Y-\0" "  |\0" " So\0"
    " ..\0" " c0\0" " ip\0" " <<\0" "  _\0" "   \0" " R0\0" "  -\0"
    "  o\0" " +-\0" "  2\0" "  3\0" "  '\0" "  u\0" " q|\0" "  .\0"
    "  ,\0" "  1\0" "  2\0" " >>\0" "1/4\0" "1/2\0" "3/4\0" "  ?\0"
    " A`\0" " A'\0" " A^\0" " A~\0" " A:\0" " Ao\0" " AE\0" " C,\0"
    " E`\0" " E'\0" " E^\0" " E:\0" " I`\0" " I'\0" " I^\0" " I:\0"
    " D-\0" " N~\0" " O`\0" " O'\0" " O^\0" " O~\0" " O:\0" "  X\0"
    " O/\0" " U`\0" " U'\0" " U^\0" " U:\0" " Y'\0" "  P\0" "  B\0"
    " a`\0" " a'\0" " a^\0" " a~\0" " a:\0" " ao\0" " ae\0" " c,\0"
    " e`\0" " e'\0" " e^\0" " e:\0" " i`\0" " i'\0" " i^\0" " i:\0"
    "  d\0" " n~\0" " o`\0" " o'\0" " o^\0" " o~\0" " o:\0" " -:\0"
    " o/\0" " u`\0" " u'\0" " u^\0" " u:\0" " y'\0" "  P\0" " y:\0";
    int ch, dh;
    int class_p;

    (void) xw;

    printf("static int charClass[256] = {\n");
    for (ch = 0; ch < 256; ++ch) {
	const char *s = charnames + (ch * 4);
	if ((ch & 7) == 0)
	    printf("/*");
	printf(" %s ", s);
	if (((ch + 1) & 7) == 0) {
	    printf("*/\n  ");
	    for (dh = ch - 7; dh <= ch; ++dh) {
		printf(" %3d%s", CharacterClass(dh), dh == 255 ? "};" : ",");
	    }
	    printf("\n");
	}
    }

    /* print the table as if it were the charClass resource */
    printf("\n");
    printf("The table is equivalent to this \"charClass\" resource:\n");
    class_p = CharacterClass(dh = 0);
    for (ch = 0; ch < 256; ++ch) {
	int class_c = CharacterClass(ch);
	if (class_c != class_p) {
	    if (show_cclass_range(dh, ch - 1)) {
		dh = ch;
		class_p = class_c;
	    }
	}
    }
    if (dh < 255) {
	show_cclass_range(dh, 255);
    }

    if_OPT_WIDE_CHARS(TScreenOf(xw), {
	/* if this is a wide-character configuration, print all intervals */
	report_wide_char_class();
    });
}