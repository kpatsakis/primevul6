static int gain_effective_cap(const char *cap_name)
{
    int cap;
    int ret = 0;

    cap = capng_name_to_capability(cap_name);
    if (cap < 0) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "capng_name_to_capability(%s) failed:%s\n",
                 cap_name, strerror(errno));
        goto out;
    }

    if (load_capng()) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "load_capng() failed\n");
        goto out;
    }

    if (capng_update(CAPNG_ADD, CAPNG_EFFECTIVE, cap)) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "capng_update(ADD,) failed\n");
        goto out;
    }

    if (capng_apply(CAPNG_SELECT_CAPS)) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "gain:capng_apply() failed\n");
        goto out;
    }
    ret = 0;

out:
    return ret;
}