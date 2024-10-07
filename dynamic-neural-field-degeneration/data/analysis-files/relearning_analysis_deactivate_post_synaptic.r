# Load required libraries
library(ggplot2)
library(extrafont)

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

# Experiment parameters
resultPath <- '../results/'
degeneracyType <- 'deactivate post-synaptic neurons'
relearningType <- 'All-cases'
epochs <- 1
maximumLearningCycles <- 200
updateAllWeights <- 1  # 0 or 1

# Construct file path
filePath <- paste0(resultPath, degeneracyType, ' ', relearningType, 
                   ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                   ' Update-all-weights-', updateAllWeights, '.txt')

# Read data from the file
data <- read_data(filePath)
dataMatrix <- data$dataMatrix
maxColumns <- data$maxColumnSize

# Analysis parameters
initialPer <- 75
incPer <- 0.278
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

# Plot the evolution of failed behavior, recovered behavior, and average relearning cycles
relearning_scalar <- 90
y_axis_scale <- relearning_scalar
dot_size <- 3
alpha_plots <- 0.7
ggplot() +
  # Dot plot for Failed Behavior
  geom_point(aes(x = degenerationPercentages, y = perFailedBehaviour, color = 'Failed Behavior'), 
             size = dot_size + 0.5, alpha = alpha_plots) +
  
  # Dot plot for Recovered Behavior
  geom_point(aes(x = degenerationPercentages, y = perRecoveredBehaviour, color = 'Recovered Behavior'), 
             size = dot_size, shape = 15, alpha = alpha_plots) +
  
  # Dot plot for Average Relearning Cycles on the secondary y-axis
  geom_point(aes(x = degenerationPercentages, y = avgRelearningCycles * relearning_scalar, 
                 color = 'Average Relearning Cycles'), size = dot_size, shape = 17, alpha = alpha_plots) +
  
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
  
  # Scaling the x-axis with 0.5 incremental ticks
  scale_x_continuous(
    name = 'Degeneration Percentage (%)',
    limits = c(75, 100),  # Set limits for the x-axis
    breaks = seq(75, 100, by = 1)  # Set ticks at 0.5 intervals
  ) +
  
  # Color and theme adjustments
  scale_color_manual(values = c('Failed Behavior' = 'red', 
                                'Recovered Behavior' = 'darkgreen', 
                                'Average Relearning Cycles' = 'blue')) +
  labs(
    title = 'Behavior Evolution vs. Degeneration Percentage',
    subtitle = 'Visualization of Failed and Recovered Behavior with Average Relearning Cycles needed to recover said behavior',
    caption = 'Data represents the evolution of behavior across different degeneration percentages.',
    color = 'Legend'
  ) +
  theme_minimal(base_size = 15) +  # Use a minimal theme for a cleaner look
  theme(
    axis.title = element_text(size = 14, face = "bold"),
    axis.title.y.right = element_text(margin = margin(l = 10), size = 14, face = "bold"),  # Add margin to the right y-axis label
    legend.title = element_text(size = 14, face = "bold"),
    legend.position = "top",  # Place the legend at the top for better readability
    legend.justification = c("right"),
    legend.background = element_rect(fill = "white", color = NA),
    text = element_text(family = "EB Garamond", size = 14),
    panel.grid.major = element_line(color = "lightgray", size = 0.5),
    panel.grid.minor = element_blank(),  # Remove minor grid lines for a cleaner look
    plot.title = element_text(face = "bold", size = 16),
    plot.subtitle = element_text(size = 12, face = "italic"),
    plot.caption = element_text(size = 10, face = "italic", color = "darkgray")
  )