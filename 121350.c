show_cclass_range(int lo, int hi)
{
    int cclass = CharacterClass(lo);
    int ident = (cclass == lo);
    int more = 0;
    if (ident) {
	int ch;
	for (ch = lo + 1; ch <= hi; ch++) {
	    if (CharacterClass(ch) != ch) {
		ident = 0;
		break;
	    }
	}
	if (ident && (hi < 255)) {
	    ch = hi + 1;
	    if (CharacterClass(ch) == ch) {
		if (ch >= 255 || CharacterClass(ch + 1) != ch) {
		    more = 1;
		}
	    }
	}
    }
    if (!more) {
	if (lo == hi) {
	    printf("\t%d", lo);
	} else {
	    printf("\t%d-%d", lo, hi);
	}
	if (!ident)
	    printf(":%d", cclass);
	if (hi < 255)
	    printf(", \\");
	printf("\n");
    }
    return !more;
}