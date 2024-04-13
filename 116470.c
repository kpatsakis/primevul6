static bool fpu_allocated(struct fpu *fpu)
{
	return fpu->state != NULL;
}