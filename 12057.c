qf_list_has_valid_entries(qf_list_T *qfl)
{
    return !qf_list_empty(qfl) && !qfl->qf_nonevalid;
}