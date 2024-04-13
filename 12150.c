qf_list_empty(qf_list_T *qfl)
{
    return qfl == NULL || qfl->qf_count <= 0;
}