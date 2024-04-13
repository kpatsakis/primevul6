static inline double argtod(struct arg arg)
{
    double value;
    mystrtod(&arg.start, &value);
    return value;
}