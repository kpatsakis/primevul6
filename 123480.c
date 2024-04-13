TEST(SegmentSumOpModelTest, Float32Test_Simple) {
  SegmentSumOpModel<float> model({TensorType_FLOAT32, {3, 4}},
                                 {TensorType_INT32, {3}});
  model.PopulateTensor<float>(model.data(),
                              {1, 2, 3, 4, 4, 3, 2, 1, 5, 6, 7, 8});
  model.PopulateTensor<int>(model.segment_ids(), {0, 0, 1});
  model.Invoke();
  EXPECT_THAT(model.GetOutput(), ElementsAreArray({5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
                                                   6.0f, 7.0f, 8.0f}));
  EXPECT_THAT(model.GetOutputShape(), ElementsAreArray({2, 4}));
}