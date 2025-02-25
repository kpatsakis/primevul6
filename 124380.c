getName(struct SWF_ACTIONPUSHPARAM *act)
{
	char *t;

	switch( act->Type ) 	
	{
	case PUSH_STRING: /* STRING */
		if (!act->p.String) /* Not a NULL string */
		{
		        SWF_warn("WARNING: Call to getName with NULL string.\n");
		        break;
		}
		else if (strlen(act->p.String)) /* Not a zero length string */
		{
		        t=malloc(strlen(act->p.String)+3);
		        strcpyext(t,act->p.String);
		        return t;
		}
		else
		{
		        char *return_string = "this";
	                t=malloc(strlen(return_string)+1); /* string length + \0 */
	                strcpyext(t,return_string);
			return t;
		}
#if 0
	  case 4: /* REGISTER */
		t=malloc(5); /* Rddd */
  		sprintf(t,"R%d", act->p.RegisterNumber );
  		return t;
#endif
	case PUSH_CONSTANT: /* CONSTANT8 */
		if (act->p.Constant8 > poolcounter)
		{
		        SWF_warn("WARNING: retrieving constants not present in the pool.\n");
		        break;
		}
		t=malloc(strlenext(pool[act->p.Constant8])+1);
		strcpyext(t,pool[act->p.Constant8]);
		if(strlen(t)) /* Not a zero length string */
			return t;
		else
		{
			t=realloc(t,6);
			return strcpy(t,"this");
		}
	case PUSH_CONSTANT16: /* CONSTANT16 */
		if (act->p.Constant16 > poolcounter)
		{
		        SWF_warn("WARNING: retrieving constants not present in the pool.\n");
		        break;
		}
		t=malloc(strlenext(pool[act->p.Constant16])+1);
		strcpyext(t,pool[act->p.Constant16]);
		if(strlen(t)) /* Not a zero length string */
			return t;
		else
		{
			t=realloc(t,6);
			return strcpy(t,"this");
		}
	default: 
		return getString(act);
	}

	t = malloc(sizeof(char));
	strcpyext(t,"");

	return t;
}