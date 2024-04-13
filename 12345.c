prep_redo(
    int	    regname,
    long    num,
    int	    cmd1,
    int	    cmd2,
    int	    cmd3,
    int	    cmd4,
    int	    cmd5)
{
    prep_redo_num2(regname, num, cmd1, cmd2, 0L, cmd3, cmd4, cmd5);
}