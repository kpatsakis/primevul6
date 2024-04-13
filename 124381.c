decompileSTOREREGISTER(int n, SWF_ACTION *actions, int maxn)
{
	struct SWF_ACTIONPUSHPARAM *data;
	OUT_BEGIN2(SWF_ACTIONSTOREREGISTER);
	data=peek();

	if (!regs[sact->Register] || sact->Register==0 )	// ===internal===
	{
		regs[sact->Register] = data;
	}
	else						// ===user visible level===
	{
		if ( regs[sact->Register]->Type == PUSH_VARIABLE) // V7: a named function parameter in register
		{						// V7: a local var in register
			if (data->Type==12)
				data->Type = PUSH_VARIABLE;			// do nothing, but only once
			else
			{
				char *l=getName(regs[sact->Register]);
				char *r=getName(data);
				if (strcmp(l,r))
				{
					INDENT
					if (data->Type==11)
					{
						println("%s;", r);
					}
					else
					{
						printf("%s = ",l);
						decompilePUSHPARAM(data,1);
						println(";");
					}
				}
			}
		}
	}
	return 0;
}