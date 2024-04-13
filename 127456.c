DEFINE_IDTENTRY_RAW(xenpv_exc_nmi)
{
	/* On Xen PV, NMI doesn't use IST.  The C part is the sane as native. */
	exc_nmi(regs);
}