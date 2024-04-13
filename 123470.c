TEST(SegmentSumOpModelTest, Int32Test_Simple) {
  SegmentSumOpModel<int32_t> model({TensorType_INT32, {3, 4}},
                                   {TensorType_INT32, {3}});
  model.PopulateTensor<int32_t>(model.data(),
                                {1, 2, 3, 4, 4, 3, 2, 1, 5, 6, 7, 8});
  model.PopulateTensor<int32_t>(model.segment_ids(), {0, 0, 1});
  model.Invoke();
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({5, 5, 5, 5, 5, 6, 7, 8}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2, 4}));
}