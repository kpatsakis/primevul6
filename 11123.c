local_df(union DateData *x)
{
    assert(complex_dat_p(x));
    assert(have_df_p(x));
    return df_utc_to_local(x->c.df, x->c.of);
}