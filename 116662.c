static rpmfi fsmIter(FD_t payload, rpmfiles files, rpmFileIter iter, void *data)
{
    rpmfi fi;
    if (payload)
	fi = rpmfiNewArchiveReader(payload, files, RPMFI_ITER_READ_ARCHIVE);
    else
	fi = rpmfilesIter(files, iter);
    if (fi && data)
	rpmfiSetOnChdir(fi, onChdir, data);
    return fi;
}