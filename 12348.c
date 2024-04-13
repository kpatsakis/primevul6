unshift_special(cmdarg_T *cap)
{
    switch (cap->cmdchar)
    {
	case K_S_RIGHT:	cap->cmdchar = K_RIGHT; break;
	case K_S_LEFT:	cap->cmdchar = K_LEFT; break;
	case K_S_UP:	cap->cmdchar = K_UP; break;
	case K_S_DOWN:	cap->cmdchar = K_DOWN; break;
	case K_S_HOME:	cap->cmdchar = K_HOME; break;
	case K_S_END:	cap->cmdchar = K_END; break;
    }
    cap->cmdchar = simplify_key(cap->cmdchar, &mod_mask);
}