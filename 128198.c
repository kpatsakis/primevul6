reset_last_sourcing(void)
{
    VIM_CLEAR(last_sourcing_name);
    last_sourcing_lnum = 0;
}