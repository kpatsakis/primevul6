add_to_showcmd_c(int c)
{
    if (!add_to_showcmd(c))
	setcursor();
}