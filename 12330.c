prep_redo_num2(
    int	    regname,
    long    num1,
    int	    cmd1,
    int	    cmd2,
    long    num2,
    int	    cmd3,
    int	    cmd4,
    int	    cmd5)
{
    ResetRedobuff();
    if (regname != 0)	// yank from specified buffer
    {
	AppendCharToRedobuff('"');
	AppendCharToRedobuff(regname);
    }
    if (num1 != 0)
	AppendNumberToRedobuff(num1);
    if (cmd1 != NUL)
	AppendCharToRedobuff(cmd1);
    if (cmd2 != NUL)
	AppendCharToRedobuff(cmd2);
    if (num2 != 0)
	AppendNumberToRedobuff(num2);
    if (cmd3 != NUL)
	AppendCharToRedobuff(cmd3);
    if (cmd4 != NUL)
	AppendCharToRedobuff(cmd4);
    if (cmd5 != NUL)
	AppendCharToRedobuff(cmd5);
}