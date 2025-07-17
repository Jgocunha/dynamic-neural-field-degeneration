# Load required libraries
library(ggplot2)
library(extrafont)
library(dplyr)
library(tidyr)
library(svglite)
library(plotrix)

#font_import()
loadfonts(device="all")

# List available Windows fonts
fonts <- windowsFonts()
#print(names(fonts))

# Get the script directory
if (interactive()) {
  script_directory <- rstudioapi::getActiveDocumentContext()$path
  script_directory <- dirname(script_directory)
} else {
  script_directory <- getwd()  # Fallback to current working directory
}
setwd(script_directory)

# Load the data
current_directory <- getwd()

# Define the function to read data from the file
read_data <- function(filePath) {
  # Read the file into a matrix
  dataMatrix <- as.matrix(read.table(filePath, header = FALSE, fill = TRUE))
  
  # Determine the maximum number of columns
  maxColumnSize <- ncol(dataMatrix)
  
  return(list(dataMatrix = dataMatrix, maxColumnSize = maxColumnSize))
}

# Set experiment parameters
resultPath <- '../results/'
degeneracyType <- 'deactivate post-synaptic neurons'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 50
updateAllWeights <- 0

# Construct file path
filePath <- paste0(resultPath, degeneracyType, ' ', relearningType, 
                   '  Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                   ' Update-all-weights-', updateAllWeights, '.txt')

# Read data from the file
data <- read_data(filePath)
dataMatrix <- data$dataMatrix
maxColumns <- data$maxColumnSize

# Analysis parameters
initialPer <- 80
finalPer <- 100
incPer <- 0.36
finalPer <- initialPer + (incPer * maxColumns) - 1 * incPer
degenerationPercentages <- seq(initialPer, finalPer, by = incPer)

# Initialize result variables
numCorrectBehaviour <- rep(0, ncol(dataMatrix))
numFailedBehaviour <- rep(0, ncol(dataMatrix))
numRecoveredBehaviour <- rep(0, ncol(dataMatrix))
numDeadFields <- rep(0, ncol(dataMatrix))
avgRelearningCycles <- rep(0, ncol(dataMatrix))
stdErrRelearningCycles <- rep(0, ncol(dataMatrix))
numValidElements <- rep(0, ncol(dataMatrix)) # variable for counting valid elements used in relearning cycles

perCorrectBehaviour <- rep(0, ncol(dataMatrix))
perFailedBehaviour <- rep(0, ncol(dataMatrix))
perRecoveredBehaviour <- rep(0, ncol(dataMatrix))
perDeadFields <- rep(0, ncol(dataMatrix))

# Iterate through each column (degeneration percentage)
for (col in 1:ncol(dataMatrix)) {
  columnData <- dataMatrix[, col]
  
  # Count the number of "dead" fields (NaN values)
  numDeadFields[col] <- sum(is.na(columnData))
  
  # Filter out NA values
  validData <- columnData[!is.na(columnData)]
  
  # Count the number of failed behaviours
  numFailedBehaviour[col] <- sum(validData > 0) + numDeadFields[col]
  
  # Count recovered behaviour (values between 0 and maximumLearningCycles)
  numRecoveredBehaviour[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  
  # Count correct behaviours (not failed)
  numCorrectBehaviour[col] <- nrow(dataMatrix) - numFailedBehaviour[col]
  
  # Calculate average relearning cycles
  avgRelearningCycles[col] <- mean(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
  # Calculate standard deviation for relearning cycles
  stdErrRelearningCycles[col] <- std.error(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
  # Count valid elements used in the average and std dev calculations
  numValidElements[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  
  # Calculate percentages
  perCorrectBehaviour[col] <- numCorrectBehaviour[col] / nrow(dataMatrix) * 100
  perRecoveredBehaviour[col] <- numRecoveredBehaviour[col] / numFailedBehaviour[col] * 100
  perFailedBehaviour[col] <- numFailedBehaviour[col] / nrow(dataMatrix) * 100
  perDeadFields[col] <- numDeadFields[col] / nrow(dataMatrix) * 100
}

# Create a data frame to hold the results
results <- data.frame(
  Degeneracy = degenerationPercentages,
  CorrectBehaviour = perCorrectBehaviour,
  FailedBehaviour = perFailedBehaviour,
  RecoveredBehaviour = perRecoveredBehaviour,
  AvgRelearningCycles = avgRelearningCycles,
  StdErrRelearningCycles = stdErrRelearningCycles,
  NumValidElements <- numValidElements,
  DeadFields = perDeadFields
)

# Behavior percentage(%) y-axis scaling
bp_max <- 100
bp_min <- 0

# Filter the results to include only degeneration percentages from initialPer% onwards
results_filtered <- results %>%
  filter(Degeneracy >= initialPer)

# Define a scaling factor for the secondary axis
relearning_scalar <- 20
y_axis_scale <- relearning_scalar

# Display the table
print(results)

# Reshape the filtered results data frame to long format for behaviour percentages
results_long <- results_filtered %>%
  select(Degeneracy, 
         #CorrectBehaviour, 
         FailedBehaviour, 
         RecoveredBehaviour) %>%
  pivot_longer(cols = -Degeneracy, names_to = "BehaviourType", values_to = "Percentage")

# Filter the results to include only rows with non-zero AvgRelearningCycles
results_filtered_non_zero <- results_filtered %>%
  filter(AvgRelearningCycles > 0)

# Add NumValidElements to results_filtered_non_zero after filtering
results_filtered_non_zero$NumValidElements <- results_filtered_non_zero$NumValidElements

# Filter rows where AvgRelearningCycles == 0 for a different geom_point layer
results_filtered_zero <- results_filtered %>%
  filter(AvgRelearningCycles == 0)

# Font parameters
font <- "Garamond"
font_size <- 24

# Create the bar chart with average relearning cycles
ggplot() +
  # Bar plot for behaviour percentages
  geom_bar(data = results_long, aes(x = Degeneracy, y = Percentage, fill = BehaviourType), 
           stat = "identity", position = "dodge", color = "white") +
  
  # Plot for average relearning cycles (correctly scaled for secondary axis starting at 1)
  geom_point(data = results_filtered_non_zero, 
             aes(x = Degeneracy, y = (AvgRelearningCycles - 1) * y_axis_scale, 
                 shape = "Mean Relearning Cycles"),  # Add shape aesthetic for legend
             color = "#4A4A4A", size = 5, fill = "#4A4A4A", stroke = 1.5, alpha = 0.8) +
  
  geom_point(data = results_filtered_zero, 
             aes(x = Degeneracy, y = AvgRelearningCycles * relearning_scalar,
                 shape = "Mean Relearning Cycles"), 
             color = "#4A4A4A", size = 3, fill = "#4A4A4A", stroke = 1.5, alpha = 0.8) +
  
  # Add error bars for standard deviation
  geom_errorbar(data = results_filtered_non_zero, 
                aes(x = Degeneracy, 
                    ymin = (AvgRelearningCycles - 1 - StdErrRelearningCycles) * y_axis_scale, 
                    ymax = (AvgRelearningCycles - 1 + StdErrRelearningCycles) * y_axis_scale), 
                width = 0.2, color = "#4A4A4A", alpha = 0.8) +
  
  # Scaling the secondary y-axis with aligned tick marks
  scale_y_continuous(
    name = 'Behaviour (%)',
    limits = c(0, 100),  # Set limits for the left y-axis
    breaks = seq(0, 100, by = 20),  # Tick marks every 20 units
    sec.axis = sec_axis(
      ~ . / y_axis_scale + 1,  # Adjust the scaling so that the secondary y-axis starts from 1
      name = 'Relearning Cycles',
      breaks = seq(1, bp_max / y_axis_scale + 1, by = 20 / y_axis_scale),  # Match the breaks on the secondary axis
      labels = scales::number_format(accuracy = 0.01)  # Format with two decimal places
    )
  ) +
  labs(x = "Degeneration (%)",
       y = "Percentage (%)",
       fill = "Behaviour Type",
       shape = "") +  # Empty title for shape legend
  theme_minimal(base_size = 15) +
  theme(
    panel.grid.major = element_line(color = "lightgray", size = 0.5),
    panel.grid.minor = element_blank(),
    panel.border = element_blank(),
    text = element_text(family = font, size = font_size),
    legend.position = c(0.05, 0.9),  # Top-left inside the plot
    legend.justification = c("left", "top"),  # Aligns the legend to the top-left corner
    legend.background = element_rect(fill = alpha("white", 0.7), color = NA, size = 0.5),  # Semi-transparent white background
    legend.title = element_blank(),
    legend.text = element_text(size = font_size),
    axis.title = element_text(face = "bold"),
    axis.text = element_text(size = font_size),
    axis.title.y.right = element_text(margin = margin(l = 10)),  # Add space between label and axis values
    legend.box = "vertical",  # Stack legends vertically
    legend.spacing.y = unit(0.1, "cm"),  # Reduce spacing between legend items
    legend.margin = margin(0, 0, 0, 0),  # Remove margins around legend
    legend.box.spacing = unit(0.00, "cm"), # Reduce spacing between legend boxes
    legend.key.height = unit(0.8, "cm"),  # Reduce height of legend keys
    legend.key.width = unit(0.8, "cm")    # Reduce width of legend keys
  ) +
  scale_fill_manual(values = c(
    "RecoveredBehaviour" = "#B7E4D9",  # Green
    "FailedBehaviour" = "#F4B3B4"   # Light Red
  ),
  labels = c(
    "Failed behaviour", 
    "Recovered behaviour")) +
  scale_shape_manual(values = c("Mean Relearning Cycles" = 18),  # Diamond shape
                     labels = c("Mean Relearning Cycles" = "Mean relearning cycles")) +
  scale_x_continuous(breaks = seq(initialPer, finalPer+2, by = 2)) +
  guides(
    fill = guide_legend(order = 1, override.aes = list(shape = NA)),
    shape = guide_legend(order = 2, override.aes = list(fill = "#4A4A4A", color = "#4A4A4A"))
  ) +
  theme(
    legend.spacing.y = unit(0.1, "cm"),  # Reduce spacing between legend items
    legend.margin = margin(0, 0, 0, 0),  # Remove margins around legend
    legend.box.spacing = unit(0.1, "cm") # Reduce spacing between legend boxes
  )

new_width <- 20      # New width in inches
new_height <- 4.5

# Construct the filename based on the experiment parameters
plot_filename <- paste0("./plots/", degeneracyType, ' ', relearningType, 
                        ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                        ' Update-all-weights-', updateAllWeights, '.svg')

# Save the plot with the updated dimensions
ggsave(
  filename = plot_filename,
  plot = last_plot(),  # Save the most recent plot
  device = "svg",      # Save as SVG
  width = new_width,   # Use the new width
  height = new_height, # Use the calculated height
  units = "in"        # Specify inches for size
)

# Print the plot file path to verify where it's saved
print(plot_filename)

# Construct the filename for the results table
results_filename <- paste0("./analysis/", degeneracyType, ' ', relearningType, 
                           ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                           ' Update-all-weights-', updateAllWeights, '.txt')

# Export the results table to a text file
write.table(
  results,                # The data frame to write
  file = results_filename, # The output file name
  sep = "\t",            # Use tab as the separator
  row.names = FALSE,     # Do not include row names
  col.names = TRUE,      # Include column names
  quote = TRUE           # Quote character strings
)

# Print the results file path to verify where it's saved
print(results_filename)