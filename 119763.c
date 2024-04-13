static inline void push_arg(struct arg *args, int *nargs, char *start, char *end)
{
    if (*nargs <= MAX_VALID_NARGS) {
        rskip_spaces(&end, start);
        if (end > start) {
            args[*nargs] = (struct arg) {start, end};
            ++*nargs;
        }
    }
}