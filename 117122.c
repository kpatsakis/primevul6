static void fuse_lib_ioctl(fuse_req_t req, fuse_ino_t ino, int cmd, void *arg,
			   struct fuse_file_info *llfi, unsigned int flags,
			   const void *in_buf, size_t in_bufsz,
			   size_t out_bufsz)
{
    struct fuse *f = req_fuse_prepare(req);
    struct fuse_intr_data d;
    struct fuse_file_info fi;
    char *path, *out_buf = NULL;
    int err;

    err = -EPERM;
    if (flags & FUSE_IOCTL_UNRESTRICTED)
	goto err;

    if (flags & FUSE_IOCTL_DIR)
	get_dirhandle(llfi, &fi);
    else
	fi = *llfi;

    if (out_bufsz) {
	err = -ENOMEM;
	out_buf = malloc(out_bufsz);
	if (!out_buf)
	    goto err;
    }

    assert(!in_bufsz || !out_bufsz || in_bufsz == out_bufsz);
    if (out_buf)
	memcpy(out_buf, in_buf, in_bufsz);

    path = get_path(f, ino); /* Should be get_path_nullok() */
    if (!path) {
	err = ENOENT;
	goto err;
    }

    fuse_prepare_interrupt(f, req, &d);

	/* Note : const qualifier dropped */
    err = fuse_fs_ioctl(f->fs, path, cmd, arg, &fi, flags,
			out_buf ? (void*)out_buf : (void*)(uintptr_t)in_buf);

    fuse_finish_interrupt(f, req, &d);
    free(path);

    if (err >= 0) { /* not an error */
        fuse_reply_ioctl(req, err, out_buf, out_bufsz);
	goto out;
    }
err:
    reply_err(req, err);
out:
    free(out_buf);
}