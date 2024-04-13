_nc_captoinfo_leaks(void)
{
    if (my_string != 0) {
	FreeAndNull(my_string);
    }
    my_length = 0;
}