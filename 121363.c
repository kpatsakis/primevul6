MetaMask(XtermWidget xw)
{
#if OPT_NUM_LOCK
    unsigned meta = xw->work.meta_mods;
    if (meta == 0)
	meta = xw->work.alt_mods;
    if (meta == 0)
	meta = Mod1Mask;
#else
    unsigned meta = Mod1Mask;
    (void) xw;
#endif
    return meta;
}