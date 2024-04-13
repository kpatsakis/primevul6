  ~edge_clone_summary ()
  {
    if (prev_clone)
      edge_clone_summaries->get (prev_clone)->next_clone = next_clone;
    if (next_clone)
      edge_clone_summaries->get (next_clone)->prev_clone = prev_clone;
  }