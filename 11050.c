date_s__rfc3339(int argc, VALUE *argv, VALUE klass)
{
    VALUE str, opt;

    rb_scan_args(argc, argv, "1:", &str, &opt);
    check_limit(str, opt);

    return date__rfc3339(str);
}