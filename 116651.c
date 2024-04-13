static char * fsmFsPath(rpmfi fi, const char * suffix)
{
    return rstrscat(NULL, rpmfiDN(fi), rpmfiBN(fi), suffix ? suffix : "", NULL);
}