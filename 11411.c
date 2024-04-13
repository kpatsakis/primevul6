prof_store(state_T state)
{
    profile_end(&current);
    profile_add(&times[state], &current);
    ++counts[state];
    profile_start(&current);
}