df_to_time(int df, int *h, int *min, int *s)
{
    *h = df / HOUR_IN_SECONDS;
    df %= HOUR_IN_SECONDS;
    *min = df / MINUTE_IN_SECONDS;
    *s = df % MINUTE_IN_SECONDS;
}