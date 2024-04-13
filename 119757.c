static inline int argtoi(struct arg arg)
{
    int value;
    mystrtoi(&arg.start, &value);
    return value;
}