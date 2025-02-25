getInt(struct SWF_ACTIONPUSHPARAM *act)
{
	switch( act->Type ) 
	{
	case PUSH_FLOAT: /* FLOAT -- also used for PROPERTY storing */
		return ((int)act->p.Float);
	case PUSH_NULL: /* NULL */
		return 0;
	case PUSH_REGISTER: /* REGISTER */
		return getInt(regs[act->p.RegisterNumber]);
	case PUSH_DOUBLE: /* DOUBLE */
		return (int)act->p.Double;
	case PUSH_INT: /* INTEGER */
		return act->p.Integer;
	default: 
		fprintf (stderr,"  Can't get int for type: %d\n", act->Type);
	}
	return 0;
}