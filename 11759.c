reset_flush(void)
{
    if (my_file != 0)
	fflush(my_file);
}