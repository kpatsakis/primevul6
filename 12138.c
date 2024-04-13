qf_stack_empty(qf_info_T *qi)
{
    return qi == NULL || qi->qf_listcount <= 0;
}