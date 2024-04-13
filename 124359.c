decompileENUMERATE(int n, SWF_ACTION *actions, int maxn, int is_type2)
{
	int i=0;
	while (i < maxn && i < 5 && OpCode(actions, n+i, maxn))
		i++;
	
	INDENT  
	println("/* a for-var-in  loop should follow below: */" );
	return i-1;		// preserve some code for decompileIF()... 
} 				// ... and let decompileIF() do all the dirty work ;-)