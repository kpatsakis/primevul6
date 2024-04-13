qf_getprop_title(qf_list_T *qfl, dict_T *retdict)
{
    return dict_add_string(retdict, "title", qfl->qf_title);
}