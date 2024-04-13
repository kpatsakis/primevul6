static inline long long argtoll(struct arg arg)
{
    long long value;
    mystrtoll(&arg.start, &value);
    return value;
}