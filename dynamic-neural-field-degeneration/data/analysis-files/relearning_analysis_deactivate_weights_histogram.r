# Load required libraries
library(ggplot2)
library(extrafont)
library(dplyr)
library(tidyr)
library(svglite)
library(plotrix)

#install.packages("svglite")

font_import()
loadfonts(device="all")

# List available Windows fonts
fonts <- windowsFonts()
print(names(fonts))

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
degeneracyType <- 'deactivate weights'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 200
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
incPer <- 0.5
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
  stdErrRelearningCycles[col] <- sd(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
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
  filter(Degeneracy >= 89.5)

# Define a scaling factor for the secondary axis
relearning_scalar <- 2.7
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
font <- "Times New Roman"
font_size <- 24

# Create the bar chart with average relearning cycles
ggplot() +
  # Bar plot for behaviour percentages
  geom_bar(data = results_long, aes(x = Degeneracy, y = Percentage, fill = BehaviourType), 
           stat = "identity", position = "dodge", color = "white") +
  
  # Plot for average relearning cycles (correctly scaled for secondary axis starting at 1)
  geom_point(data = results_filtered_non_zero, 
             aes(x = Degeneracy, y = (AvgRelearningCycles - 1) * y_axis_scale),  # Adjusted scaling
             color = "#4A4A4A", size = 5, fill = "#4A4A4A", stroke = 1.5, shape = 18, alpha = 0.8) +
  
  geom_point(data = results_filtered_zero, 
             aes(x = Degeneracy, y = AvgRelearningCycles * relearning_scalar), 
             color = "#4A4A4A", size = 3, fill = "#4A4A4A", stroke = 1.5, shape = 3 , alpha = 0.8) +
  
  # Add error bars for standard deviation
  geom_errorbar(data = results_filtered_non_zero, 
                aes(x = Degeneracy, 
                    ymin = (AvgRelearningCycles - 1 - StdErrRelearningCycles) * y_axis_scale, 
                    ymax = (AvgRelearningCycles - 1 + StdErrRelearningCycles) * y_axis_scale), 
                width = 0.2, color = "#4A4A4A", alpha = 0.8) +
  
  # Add text labels for the number of valid elements next to each diamond
  #geom_text(data = results_filtered_non_zero, 
  #          aes(x = Degeneracy, y = (AvgRelearningCycles - 1) * y_axis_scale, 
  #              label = NumValidElements), 
  #          vjust = -3.5, size = 3, color = "black", family = font) +  # Adjust position with vjust# Scaling the secondary y-axis with aligned tick marks
  scale_y_continuous(
    name = 'Behavior Percentage (%)',
    limits = c(0, 100),  # Set limits for the left y-axis
    breaks = seq(0, 100, by = 10),  # Tick marks every 10 units
    sec.axis = sec_axis(
      ~ . / y_axis_scale, 
      name = 'Average Relearning Cycles',
      breaks = seq(0, 100 / y_axis_scale, by = 10 / y_axis_scale)  # Match the breaks on the secondary axis
    )
  ) +
  labs(#title = "Behaviour Analysis and Average Relearning Cycles by Degeneration Percentage (89% and Above)",
       x = "Degeneration Percentage (%)",
       y = "Percentage (%)",
       fill = "Behaviour Type") +
  theme_minimal(base_size = 15) +
  theme(
    panel.grid.major = element_line(color = "lightgray", size = 0.5),
    panel.grid.minor = element_blank(),
    panel.border = element_blank(),
    text = element_text(family = font, size = font_size),
    legend.position = c(0.05, 0.9),  # Top-left inside the plot
    legend.justification = c("left", "top"),  # Aligns the legend to the top-left corner
    legend.background = element_rect(fill = "white", color = NA, size = 0.5),  # White background for clarity
    legend.title = element_blank(),
    #plot.title = element_text(hjust = 0.5, size = 20, face = "bold"),
    legend.text = element_text(size = font_size),
    axis.title = element_text(face = "bold"),
    axis.text = element_text(size = font_size),
    axis.title.y.right = element_text(margin = margin(l = 10))  # Add space between label and axis values
  )  +
  scale_fill_manual(values = c(
    #"CorrectBehaviour" = "#B3CDE0",  # Light Blue
    "RecoveredBehaviour" = "#B7E4D9",  # Green
    "FailedBehaviour" = "#F4B3B4"   # Light Red
  ),
  labels = c(
    #"Correct behaviour", 
    "Failed behaviour", 
    "Recovered behaviour")) +
  scale_x_continuous(breaks = seq(initialPer, finalPer, by = 1)) +
  # Scaling the secondary y-axis with aligned tick marks, ensuring the secondary y-axis starts at 1
  scale_y_continuous(
    name = 'Behaviour Percentage (%)',
    limits = c(bp_min, bp_max),  # Set limits for the left y-axis
    breaks = seq(bp_min, bp_max, by = 10),  # Tick marks every 10 units
    sec.axis = sec_axis(
      ~ . / y_axis_scale + 1,  # Adjust the scaling so that the secondary y-axis starts from 1
      name = 'Average Relearning Cycles',
      breaks = seq(1, bp_max / y_axis_scale + 1, by = 10 / y_axis_scale),  # Match the breaks on the secondary axis
      labels = scales::number_format(accuracy = 0.01)  # Format with two decimal places
    )
  )

new_width <- 10      # New width in inches
new_height <- (new_width / 3.5) * 3  # Calculate new height to maintain aspect ratio

# Construct the filename based on the experiment parameters
plot_filename <- paste0("./plots/", degeneracyType, ' ', relearningType, 
                        ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                        ' Update-all-weights-', updateAllWeights, '.svg')

# Save the plot with the updated dimensions
ggsave(
  filename = plot_filename,
  plot = last_plot(),  # Save the most recent plot
  device = "svg",      # Save as SVG
  #device = svglite::svglite,
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