void Con_Linefeed (qboolean skipnotify)
{
	int		i;

	// mark time for transparent overlay
	if (con.current >= 0)
	{
    if (skipnotify)
		  con.times[con.current % NUM_CON_TIMES] = 0;
    else
		  con.times[con.current % NUM_CON_TIMES] = cls.realtime;
	}

	con.x = 0;
	if (con.display == con.current)
		con.display++;
	con.current++;
	for(i=0; i<con.linewidth; i++)
		con.text[(con.current%con.totallines)*con.linewidth+i] = (ColorIndex(COLOR_WHITE)<<8) | ' ';
}