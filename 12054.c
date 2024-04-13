existing_swapfile(buf_T *buf)
{
    if (buf->b_ml.ml_mfp != NULL && buf->b_ml.ml_mfp->mf_fname != NULL)
    {
	char_u *fname = buf->b_ml.ml_mfp->mf_fname;
	size_t len = STRLEN(fname);

	return fname[len - 1] != 'p' || fname[len - 2] != 'w';
    }
    return FALSE;
}