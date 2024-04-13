bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        if (isempty(p))
                return false;

        return in_charset(p, VALID_CHARS);
}