do_select_regex(TScreen *screen, CELL *startc, CELL *endc)
{
    LineData *ld = GET_LINEDATA(screen, startc->row);
    int inx = ((screen->numberOfClicks - 1) % screen->maxClicks);
    char *expr = screen->selectExpr[inx];
    regex_t preg;
    regmatch_t match;

    TRACE(("Select_REGEX[%d]:%s\n", inx, NonNull(expr)));
    if (okPosition(screen, &ld, startc) && expr != 0) {
	if (regcomp(&preg, expr, REG_EXTENDED) == 0) {
	    int firstRow = firstRowOfLine(screen, startc->row, True);
	    int lastRow = lastRowOfLine(screen, firstRow);
	    unsigned size = lengthOfLines(screen, firstRow, lastRow);
	    int actual = cellToColumn(screen, startc);
	    int *indexed;

	    TRACE(("regcomp ok rows %d..%d bytes %d\n",
		   firstRow, lastRow, size));

	    if ((indexed = TypeCallocN(int, size + 1)) != 0) {
		char *search;
		if ((search = make_indexed_text(screen,
						firstRow,
						size,
						indexed)) != 0) {
		    int len = (int) strlen(search);
		    int col;
		    int best_col = -1;
		    int best_len = -1;

		    startc->row = 0;
		    startc->col = 0;
		    endc->row = 0;
		    endc->col = 0;

		    for (col = 0; indexed[col] < len; ++col) {
			if (regexec(&preg,
				    search + indexed[col],
				    (size_t) 1, &match, 0) == 0) {
			    int start_inx = (int) (match.rm_so + indexed[col]);
			    int finis_inx = (int) (match.rm_eo + indexed[col]);
			    int start_col = indexToCol(indexed, len, start_inx);
			    int finis_col = indexToCol(indexed, len, finis_inx);

			    if (start_col <= actual &&
				actual <= finis_col) {
				int test = finis_col - start_col;
				if (best_len < test) {
				    best_len = test;
				    best_col = start_col;
				    TRACE(("match column %d len %d\n",
					   best_col,
					   best_len));
				}
			    }
			}
		    }
		    if (best_col >= 0) {
			int best_nxt = best_col + best_len;
			columnToCell(screen, firstRow, best_col, startc);
			columnToCell(screen, firstRow, best_nxt, endc);
			TRACE(("search::%s\n", search));
			TRACE(("indexed:%d..%d -> %d..%d\n",
			       best_col, best_nxt,
			       indexed[best_col],
			       indexed[best_nxt]));
			TRACE(("matched:%d:%s\n",
			       indexed[best_nxt] + 1 -
			       indexed[best_col],
			       visibleChars((Char *) (search + indexed[best_col]),
					    (unsigned) (indexed[best_nxt] +
							1 -
							indexed[best_col]))));
		    }
		    free(search);
		}
		free(indexed);
#if OPT_DEC_CHRSET
		if ((ld = GET_LINEDATA(screen, startc->row)) != 0) {
		    if (CSET_DOUBLE(GetLineDblCS(ld)))
			startc->col *= 2;
		}
		if ((ld = GET_LINEDATA(screen, endc->row)) != 0) {
		    if (CSET_DOUBLE(GetLineDblCS(ld)))
			endc->col *= 2;
		}
#endif
	    }
	    regfree(&preg);
	}
    }
}