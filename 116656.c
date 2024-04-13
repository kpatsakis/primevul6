static void setFileState(rpmfs fs, int i)
{
    switch (rpmfsGetAction(fs, i)) {
    case FA_SKIPNSTATE:
	rpmfsSetState(fs, i, RPMFILE_STATE_NOTINSTALLED);
	break;
    case FA_SKIPNETSHARED:
	rpmfsSetState(fs, i, RPMFILE_STATE_NETSHARED);
	break;
    case FA_SKIPCOLOR:
	rpmfsSetState(fs, i, RPMFILE_STATE_WRONGCOLOR);
	break;
    case FA_TOUCH:
	rpmfsSetState(fs, i, RPMFILE_STATE_NORMAL);
	break;
    default:
	break;
    }
}