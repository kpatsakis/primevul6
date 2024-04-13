add_digit(int *target, int source)
{
    *target = (*target * 10) + (source - '0');
}