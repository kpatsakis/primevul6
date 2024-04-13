storageVolWipe(virStorageVolPtr vol,
               unsigned int flags)
{
    return storageVolWipePattern(vol, VIR_STORAGE_VOL_WIPE_ALG_ZERO, flags);
}