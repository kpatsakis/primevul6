reset_start(FILE *fp, bool is_reset, bool is_init)
{
    my_file = fp;
    use_reset = is_reset;
    use_init = is_init;
}