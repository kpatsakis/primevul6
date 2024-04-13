static int extend_contents(struct fuse_dh *dh, unsigned minsize)
{
    if (minsize > dh->size) {
        char *newptr;
        unsigned newsize = dh->size;
        if (!newsize)
            newsize = 1024;
#ifndef __SOLARIS__
        while (newsize < minsize) {
	    if (newsize >= 0x80000000)
	       	newsize = 0xffffffff;
	    else
	       	newsize *= 2;
        }
#else /* __SOLARIS__ */
        while (newsize < minsize)
            newsize *= 2;
#endif /* __SOLARIS__ */

        newptr = (char *) realloc(dh->contents, newsize);
        if (!newptr) {
            dh->error = -ENOMEM;
            return -1;
        }
        dh->contents = newptr;
        dh->size = newsize;
    }
    return 0;
}