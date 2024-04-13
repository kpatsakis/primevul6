expandFormat(XtermWidget xw,
	     const char *format,
	     char *data,
	     CELL *start,
	     CELL *finish)
{
    char *result = 0;
    if (!IsEmpty(format)) {
	static char empty[1];
	int pass;
	int n;
	char numbers[80];

	if (data == 0)
	    data = empty;

	for (pass = 0; pass < 2; ++pass) {
	    size_t need = 0;

	    for (n = 0; format[n] != '\0'; ++n) {

		if (format[n] == '%') {
		    char *value = 0;

		    switch (format[++n]) {
		    case '%':
			if (pass) {
			    result[need] = format[n];
			}
			++need;
			break;
		    case 'P':
			sprintf(numbers, "%d;%d",
				TScreenOf(xw)->topline + start->row + 1,
				start->col + 1);
			value = numbers;
			break;
		    case 'p':
			sprintf(numbers, "%d;%d",
				TScreenOf(xw)->topline + finish->row + 1,
				finish->col + 1);
			value = numbers;
			break;
		    case 'R':
			value = formatStrlen(numbers, x_strrtrim(data), 1);
			break;
		    case 'r':
			value = x_strrtrim(data);
			break;
		    case 'S':
			value = formatStrlen(numbers, data, 0);
			break;
		    case 's':
			value = data;
			break;
		    case 'T':
			value = formatStrlen(numbers, x_strtrim(data), 1);
			break;
		    case 't':
			value = x_strtrim(data);
			break;
		    case 'V':
			formatVideoAttrs(xw, numbers, start);
			value = numbers;
			break;
		    case 'v':
			formatVideoAttrs(xw, numbers, finish);
			value = numbers;
			break;
		    default:
			if (pass) {
			    result[need] = format[n];
			}
			--n;
			++need;
			break;
		    }
		    if (value != 0) {
			if (pass) {
			    strcpy(result + need, value);
			}
			need += strlen(value);
			if (value != numbers && value != data) {
			    free(value);
			}
		    }
		} else {
		    if (pass) {
			result[need] = format[n];
		    }
		    ++need;
		}
	    }
	    if (pass) {
		result[need] = '\0';
	    } else {
		++need;
		result = malloc(need);
		if (result == 0) {
		    break;
		}
	    }
	}
    }
    TRACE(("expandFormat(%s) = %s\n", NonNull(format), NonNull(result)));
    return result;
}