int unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        int r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        if (r < 0)
                return r;

        return unit_name_path_unescape(prefix, ret);
}