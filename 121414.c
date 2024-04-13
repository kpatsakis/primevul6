CutBuffer(Atom code)
{
    int cutbuffer;
    switch ((unsigned) code) {
    case XA_CUT_BUFFER0:
	cutbuffer = 0;
	break;
    case XA_CUT_BUFFER1:
	cutbuffer = 1;
	break;
    case XA_CUT_BUFFER2:
	cutbuffer = 2;
	break;
    case XA_CUT_BUFFER3:
	cutbuffer = 3;
	break;
    case XA_CUT_BUFFER4:
	cutbuffer = 4;
	break;
    case XA_CUT_BUFFER5:
	cutbuffer = 5;
	break;
    case XA_CUT_BUFFER6:
	cutbuffer = 6;
	break;
    case XA_CUT_BUFFER7:
	cutbuffer = 7;
	break;
    default:
	cutbuffer = -1;
	break;
    }
    TRACE2(("CutBuffer(%d) = %d\n", (int) code, cutbuffer));
    return cutbuffer;
}