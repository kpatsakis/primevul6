is_ident(char_u *line, int offset)
{
    int	i;
    int	incomment = FALSE;
    int	instring = 0;
    int	prev = 0;

    for (i = 0; i < offset && line[i] != NUL; i++)
    {
	if (instring != 0)
	{
	    if (prev != '\\' && line[i] == instring)
		instring = 0;
	}
	else if ((line[i] == '"' || line[i] == '\'') && !incomment)
	{
	    instring = line[i];
	}
	else
	{
	    if (incomment)
	    {
		if (prev == '*' && line[i] == '/')
		    incomment = FALSE;
	    }
	    else if (prev == '/' && line[i] == '*')
	    {
		incomment = TRUE;
	    }
	    else if (prev == '/' && line[i] == '/')
	    {
		return FALSE;
	    }
	}

	prev = line[i];
    }

    return incomment == FALSE && instring == 0;
}