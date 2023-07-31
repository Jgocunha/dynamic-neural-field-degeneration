#include "./user_interface/matrix_plot_window.h"

MatrixPlotWindow::MatrixPlotWindow(const std::shared_ptr<Simulation> simulation,  const std::string& couplingUniqueId)
{
	coupling = std::dynamic_pointer_cast<FieldCoupling>(simulation->getElement(couplingUniqueId));
}

void MatrixPlotWindow::render()
{
	if (ImGui::Begin("Weight visualization"))
	{
        displayHeatMap();
	}
	ImGui::End();
}


void MatrixPlotWindow::displayHeatMap()
{
    std::vector<std::vector<double>> weights = coupling->getWeights();

    // Define the dimensions of the original matrix
    int originalRows = static_cast<int>(weights.size());
    int originalCols = static_cast<int>(weights[0].size());

    // Define the dimensions of the resampled matrix
    static int resampledRows = 360;
    static int resampledCols = 180;

    ImGui::Text("This weight matrix is originally [%i; %i], but was resampled to [%i; %i].", originalRows, originalCols, resampledRows, resampledCols);
    
    const float dragWidth = 100.0f; // Adjust the width as needed
    // Label for the "Size rows" drag int box
    ImGui::Text("Size rows: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(dragWidth);
    ImGui::DragInt("##SizeRows", &resampledRows, 1.0f, 0, originalRows);

    // Label for the "Size cols" drag int box
    ImGui::Text("Size cols: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(dragWidth);
    ImGui::DragInt("##SizeCols", &resampledCols, 1.0f, 0, originalCols);


    // Calculate the row and column ratios between the original and resampled matrix
    double rowRatio = static_cast<double>(originalRows) / static_cast<double>(resampledRows);
    double colRatio = static_cast<double>(originalCols) / static_cast<double>(resampledCols);

    // Create the resampled matrix
    std::vector<std::vector<double>> resampledWeights(resampledRows, std::vector<double>(resampledCols, 0.0));

    // Resample the data
    for (int r = 0; r < resampledRows; ++r) {
        for (int c = 0; c < resampledCols; ++c) {
            // Calculate the range of rows and columns to average from the original matrix
            int startRow = static_cast<int>(r * rowRatio);
            int endRow = static_cast<int>((r + 1) * rowRatio);
            int startCol = static_cast<int>(c * colRatio);
            int endCol = static_cast<int>((c + 1) * colRatio);

            // Average the values in the specified range
            double sum = 0.0;
            int count = 0;
            for (int i = startRow; i < endRow; ++i) {
                for (int j = startCol; j < endCol; ++j) {
                    sum += weights[i][j];
                    ++count;
                }
            }
            resampledWeights[r][c] = sum / static_cast<double>(count);
        }
    }

    // Convert the resampled weights matrix to a 2D array for ImPlot
    int rows = static_cast<int>(resampledWeights.size());
    int cols = static_cast<int>(resampledWeights[0].size());
    float* data = new float[rows * cols];
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            data[i * cols + j] = resampledWeights[i][j];
        }
    }

    // ... Continue with the rest of your plotting code ...


    static float scale_min = -0.1;
    static float scale_max = 0.19;

    static ImPlotColormap map = ImPlotColormap_Viridis;
    if (ImPlot::ColormapButton(ImPlot::GetColormapName(map), ImVec2(225, 0), map)) {
        map = (map + 1) % ImPlot::GetColormapCount();
        // We bust the color cache of our plots so that item colors will
        // resample the new colormap in the event that they have already
        // been created. See documentation in implot.h.
        //BustColorCache("##Heatmap1");
    }

    ImGui::SameLine();
    ImGui::LabelText("##Colormap Index", "%s", "Change Colormap");
    ImGui::SetNextItemWidth(225);
    ImGui::DragFloatRange2("Min / Max", &scale_min, &scale_max, 0.005f, -20, 20);

    static ImPlotHeatmapFlags hm_flags = 0;
    
    ImPlot::PushColormap(map);

    // Calculate the desired plot size based on resampledCols and resampledRows
    float plotWidth = static_cast<float>(resampledCols) * 25.0f; // You can adjust the scale factor as needed
    float plotHeight = static_cast<float>(resampledRows) * 25.0f; // You can adjust the scale factor as needed

    // Ensure that the plot size is within a reasonable range
    plotWidth = std::max(plotWidth, 400.0f);
    plotHeight = std::max(plotHeight, 300.0f);

    if (ImPlot::BeginPlot("##Heatmap1", ImVec2(plotHeight,plotWidth), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText))
    {
        ImPlot::PlotHeatmap("Weights", data, rows, cols, scale_min, scale_max, "%.2f", ImPlotPoint(0, 0), ImPlotPoint(resampledCols - 1, resampledRows - 1), hm_flags);
        ImPlot::EndPlot();
    }
    ImGui::SameLine();
    ImPlot::ColormapScale("##HeatScale", scale_min, scale_max, ImVec2(60, 225));

    ImGui::SameLine();

    // Don't forget to clean up the dynamically allocated data
    delete[] data;
}