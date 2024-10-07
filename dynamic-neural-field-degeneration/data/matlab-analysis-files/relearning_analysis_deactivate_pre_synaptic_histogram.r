# Load required libraries
library(ggplot2)
library(extrafont)
library(dplyr)
library(tidyr)

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
degeneracyType <- 'deactivate pre-synaptic neurons'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 200
updateAllWeights <- 1

# Construct file path
filePath <- paste0(resultPath, degeneracyType, ' ', relearningType, 
                   '  Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                   ' Update-all-weights-', updateAllWeights, '.txt')

# Read data from the file
data <- read_data(filePath)
dataMatrix <- data$dataMatrix
maxColumns <- data$maxColumnSize

# Analysis parameters
initialPer <- 10
incPer <- 0.1389
finalPer <- initialPer + (incPer * maxColumns) - 1 * incPer
degenerationPercentages <- seq(initialPer, finalPer, by = incPer)

# Initialize result variables
numCorrectBehaviour <- rep(0, ncol(dataMatrix))
numFailedBehaviour <- rep(0, ncol(dataMatrix))
numRecoveredBehaviour <- rep(0, ncol(dataMatrix))
numDeadFields <- rep(0, ncol(dataMatrix))
avgRelearningCycles <- rep(0, ncol(dataMatrix))

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
  avgRelearningCycles[col] <- mean(validData[validData < maximumLearningCycles], na.rm = TRUE)
  
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
  DeadFields = perDeadFields
)

# Display the table
print(results)

# Filter the results to include only degeneration percentages from 89% onwards
results_filtered <- results %>%
  filter(Degeneracy >= 10)

# Define a scaling factor for the secondary axis
relearning_scalar <- 30
y_axis_scale <- relearning_scalar

# Reshape the filtered results data frame to long format for behaviour percentages
results_long <- results_filtered %>%
  select(Degeneracy, CorrectBehaviour, FailedBehaviour, RecoveredBehaviour) %>%
  pivot_longer(cols = -Degeneracy, names_to = "BehaviourType", values_to = "Percentage")

# Create the bar chart with average relearning cycles
ggplot() +
  # Bar plot for behaviour percentages
  geom_bar(data = results_long, aes(x = Degeneracy, y = Percentage, fill = BehaviourType), 
           stat = "identity", position = "dodge", color = "white") +
  # Plot for average relearning cycles
  #geom_line(data = results_filtered, aes(x = Degeneracy, y = AvgRelearningCycles * 100 / max(AvgRelearningCycles), 
   #                                      group = 1), color = "black", size = 1) +  # Scale to percentage
  geom_point(data = results_filtered, aes(x = Degeneracy, y = AvgRelearningCycles * relearning_scalar), 
                 color = "#4A4A4A", size = 5, fill = "#4A4A4A",stroke = 1.5, shape = 18 , alpha = 0.8) +
  # Scaling the secondary y-axis with aligned tick marks
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
  labs(title = "Behaviour Analysis and Average Relearning Cycles by Degeneration Percentage (89% and Above)",
       x = "Degeneration Percentage (%)",
       y = "Percentage (%)",
       fill = "Behaviour Type") +
  theme_minimal(base_size = 15) +
  theme(
    panel.grid.major = element_line(color = "lightgray", size = 0.5),
    panel.grid.minor = element_blank(),
    panel.border = element_blank(),
    legend.position = "top",
    text = element_text(family = "EB Garamond", size = 14),
    legend.title = element_blank(),
    plot.title = element_text(hjust = 0.5, size = 20, face = "bold"),
    axis.title = element_text(face = "bold"),
    axis.text = element_text(size = 12)
  )  +
  scale_fill_manual(values = c(
    "CorrectBehaviour" = "#B3CDE0",  # Light Blue
    "RecoveredBehaviour" = "#B7E4D9",  # Green
    "FailedBehaviour" = "#F4B3B4"   # Light Red
  ),
  labels = c("Correct behaviour", "Failed behaviour", "Recovered behaviour")) +
  scale_x_continuous(breaks = seq(10, 20, by = 1)) +
  scale_y_continuous(
    name = 'Behaviour Percentage (%)',
    limits = c(0, 100),  # Set limits for the left y-axis
    breaks = seq(0, 100, by = 10),  # Tick marks every 10 units
    sec.axis = sec_axis(
      ~ . / y_axis_scale,  # Scale back to average relearning cycles
      name = 'Average Relearning Cycles',
      breaks = seq(0, 100 / y_axis_scale, by = 10 / y_axis_scale)  # Match the breaks on the secondary axis
    ))
