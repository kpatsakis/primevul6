repeated_append(string_desc * target, int total, int num, int repeat, const char *src)
{
    size_t need = (size_t) repeat * strlen(src);

    if (need < target->s_size) {
	while (repeat-- > 0) {
	    if (_nc_safe_strcat(target, src)) {
		total += num;
	    } else {
		total = INFINITY;
		break;
	    }
	}
    } else {
	total = INFINITY;
    }
    return total;
}