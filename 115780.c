debug_print_range(const char *msg,
    int lineno,
    void *start, signed long len,
    void *end)
{

    char *st = (char *)start;
    char *en = (char *)end;
    signed long le = len;

    if (len) {
        if (en) {
            le = (long)(en-st);
        } else {
            en= start+len;
        }
    } else if (en) {
        le = (long)(en-st);
    }
    printf("RANGEdebug %s  st=0x%lx le=%ld en=0x%lx line %d\n",
        msg,(unsigned long)st,le,(unsigned long)en,lineno);
}