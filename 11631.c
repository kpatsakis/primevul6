visit_nodes(const void *nodep, const VISIT which, const int depth)
{
    (void) depth;
    if (which == preorder || which == leaf) {
	delete_tparm[which_tparm] = *(TPARM_DATA **) nodep;
	which_tparm++;
    }
}