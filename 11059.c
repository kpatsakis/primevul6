c_jd_to_wday(int jd)
{
    return MOD(jd + 1, 7);
}