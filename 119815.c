static int drop_effective_cap(const char *cap_name, bool *cap_dropped)
{
    int cap, ret;

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

    /* We dont have this capability in effective set already. */
    if (!capng_have_capability(CAPNG_EFFECTIVE, cap)) {
        ret = 0;
        goto out;
    }

    if (capng_update(CAPNG_DROP, CAPNG_EFFECTIVE, cap)) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "capng_update(DROP,) failed\n");
        goto out;
    }

    if (capng_apply(CAPNG_SELECT_CAPS)) {
        ret = errno;
        fuse_log(FUSE_LOG_ERR, "drop:capng_apply() failed\n");
        goto out;
    }

    ret = 0;
    if (cap_dropped) {
        *cap_dropped = true;
    }

out:
    return ret;
}