int event_has_hard_overrides(char *str)
{
    // look for \pos and \move tags inside {...}
    // mirrors get_next_char, but is faster and doesn't change any global state
    while (*str) {
        if (str[0] == '\\' && str[1] != '\0') {
            str += 2;
        } else if (str[0] == '{') {
            str++;
            while (*str && *str != '}') {
                if (*str == '\\') {
                    char *p = str + 1;
                    if (mystrcmp(&p, "pos") || mystrcmp(&p, "move") ||
                        mystrcmp(&p, "clip") || mystrcmp(&p, "iclip") ||
                        mystrcmp(&p, "org") || mystrcmp(&p, "pbo") ||
                        mystrcmp(&p, "p"))
                        return 1;
                }
                str++;
            }
        } else {
            str++;
        }
    }
    return 0;
}