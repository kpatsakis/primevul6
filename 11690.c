ranf(void)
{
    long r = (rand() & 077777);
    return ((double) r / 32768.);
}