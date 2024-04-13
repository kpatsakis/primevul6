c_gregorian_leap_p(int y)
{
    return (MOD(y, 4) == 0 && y % 100 != 0) || MOD(y, 400) == 0;
}