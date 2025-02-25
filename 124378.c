getString(struct SWF_ACTIONPUSHPARAM *act)
{
	char *t;
#ifdef DEBUG
	printf("*getString* type=%d\n",act->Type);
#endif
	switch( act->Type ) 
	{
	case PUSH_STRING: 
		if (!act->p.String) /* Not a NULL string */
		{
		        SWF_warn("WARNING: Call to getString with NULL string.\n");
		        break;
		}
		t=malloc(strlen(act->p.String)+3); /* 2 "'"s and a NULL */
		strcpy(t,"'");
		strcat(t,act->p.String);
		strcat(t,"'");
		return t;
	case PUSH_NULL: /* NULL */
		return "null";
	case PUSH_UNDEF: /* Undefined */
		return "undefined";
	case PUSH_REGISTER: /* REGISTER */
		if( regs[act->p.RegisterNumber] &&
		    regs[act->p.RegisterNumber]->Type != 4 &&
		    regs[act->p.RegisterNumber]->Type != 7 )
		{
			return getName(regs[act->p.RegisterNumber]);
		}
		else
		{
			t=malloc(5); /* Rddd */
			sprintf(t,"R%d", act->p.RegisterNumber );
			return t;
		}
	case PUSH_BOOLEAN: /* BOOLEAN */
		if( act->p.Boolean )
			return "true";
		else
			return "false";
	case PUSH_DOUBLE: /* DOUBLE */
	{
		char length_finder[1];
		int needed_length = snprintf(length_finder, 1, "%g", act->p.Double) + 1;
		if (needed_length <= 0)
		{
		        SWF_warn("WARNING: could not evaluate size of buffer (memory issue ?).\n");
		        break;
		}

		t = malloc(needed_length);
		sprintf(t, "%g", act->p.Double );
		return t;
	}
	case PUSH_INT: /* INTEGER */
		t=malloc(10); /* 32-bit decimal */
		sprintf(t,"%ld", act->p.Integer );
		return t;
	case PUSH_CONSTANT: /* CONSTANT8 */
		if (act->p.Constant8 > poolcounter)
		{
		        SWF_warn("WARNING: retrieving constants not present in the pool.\n");
		        break;
		}
		t=malloc(strlenext(pool[act->p.Constant8])+3); /* 2 "'"s and a NULL */
		strcpy(t,"'");
		strcatext(t,pool[act->p.Constant8]);
		strcat(t,"'");
		return t;
	case PUSH_CONSTANT16: /* CONSTANT16 */
		if (act->p.Constant16 > poolcounter)
		{
		        SWF_warn("WARNING: retrieving constants not present in the pool.\n");
		        break;
		}
		t=malloc(strlenext(pool[act->p.Constant16])+3); /* 2 '\"'s and a NULL */
		strcpy(t,"'");
		strcatext(t,pool[act->p.Constant16]);
		strcat(t,"'");
		return t;

	case 12:
	case 11: /* INCREMENTED or DECREMENTED VARIABLE */
	case PUSH_VARIABLE: /* VARIABLE */
		return act->p.String;
	default: 
		fprintf (stderr,"  Can't get string for type: %d\n", act->Type);
		break;
	}

	t = malloc(sizeof(char));
	strcpyext(t,"");

	return t;
}