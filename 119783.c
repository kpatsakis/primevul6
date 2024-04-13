static int is_safe_path_component(const char *path)
{
    if (strchr(path, '/')) {
        return 0;
    }

    return !is_dot_or_dotdot(path);
}