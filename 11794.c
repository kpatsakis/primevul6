check_aliases(char *name, bool program)
{
    static char my_init[] = "init";
    static char my_reset[] = "reset";
    static char my_clear[] = "clear";

    char *result = name;
    if ((is_init = same_program(name, program ? PROG_INIT : my_init)))
	result = my_init;
    if ((is_reset = same_program(name, program ? PROG_RESET : my_reset)))
	result = my_reset;
    if ((is_clear = same_program(name, program ? PROG_CLEAR : my_clear)))
	result = my_clear;
    return result;
}