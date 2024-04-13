storageVolGetInfo(virStorageVolPtr vol,
                  virStorageVolInfoPtr info)
{
    return storageVolGetInfoFlags(vol, info, 0);
}