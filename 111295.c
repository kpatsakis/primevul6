    LineCompositeTask ( TaskGroup* group ,
                        CompositeDeepScanLine::Data * data,
                    int y,
                    int start,
                    vector<const char*>* names,
                    vector<vector< vector<float *> > >* pointers,
                    vector<unsigned int>* total_sizes,
                    vector<unsigned int>* num_sources
                  ) : Task(group) ,
                     _Data(data),
                     _y(y),
                     _start(start),
                     _names(names),
                     _pointers(pointers),
                     _total_sizes(total_sizes),
                     _num_sources(num_sources)
                     {}