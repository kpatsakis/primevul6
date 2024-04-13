static float calcRMSE(Mat flow1, Mat flow2)
{
    float sum = 0;
    int counter = 0;
    const int rows = flow1.rows;
    const int cols = flow1.cols;

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            Vec2f flow1_at_point = flow1.at<Vec2f>(y, x);
            Vec2f flow2_at_point = flow2.at<Vec2f>(y, x);

            float u1 = flow1_at_point[0];
            float v1 = flow1_at_point[1];
            float u2 = flow2_at_point[0];
            float v2 = flow2_at_point[1];

            if (isFlowCorrect(u1) && isFlowCorrect(u2) && isFlowCorrect(v1) && isFlowCorrect(v2))
            {
                sum += (u1 - u2) * (u1 - u2) + (v1 - v2) * (v1 - v2);
                counter++;
            }
        }
    }
    return (float)sqrt(sum / (1e-9 + counter));
}