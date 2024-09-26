# Clear R environment and graphics
rm(list = ls())
graphics.off()

# Load necessary libraries
library(ggplot2)

# Experiment parameters
resultPath <- '../results/'
degeneracyType <- 'deactivate pre-synaptic neurons'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 100
updateAllWeights <- 0  # 0 or 1

# File path generation
filePath <- paste0(resultPath, degeneracyType, ' ', relearningType, 
                   '  Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                   ' Update-all-weights-', updateAllWeights, '.txt')

# Function to read data
read_data <- function(filePath) {
  dataMatrix <- as.matrix(read.table(filePath, header = FALSE, fill = TRUE))
  maxColumns <- ncol(dataMatrix)
  return(list(dataMatrix = dataMatrix, maxColumns = maxColumns))
}

# Read data from file
data <- read_data(filePath)
dataMatrix <- data$dataMatrix
maxColumns <- data$maxColumns

# Analysis

# Initialize variables
initialPer <- 10
incPer <- 0.2
finalPer <- initialPer + (incPer * maxColumns) - 1 * incPer
degenerationPercentages <- seq(initialPer, finalPer, by = incPer)

numCorrectBehaviour <- rep(0, ncol(dataMatrix))  # Correct behaviour
numFailedBehaviour <- rep(0, ncol(dataMatrix))   # Failed behaviour
numRecoveredBehaviour <- rep(0, ncol(dataMatrix))  # Recovered behaviour
numDeadFields <- rep(0, ncol(dataMatrix))       # Dead fields
isDead <- 0

avgRelearningCycles <- rep(0, ncol(dataMatrix))  # Average relearning cycles

perCorrectBehaviour <- rep(0, ncol(dataMatrix))  # Correct behaviour percentage
perFailedBehaviour <- rep(0, ncol(dataMatrix))   # Failed behaviour percentage
perRecoveredBehaviour <- rep(0, ncol(dataMatrix))  # Recovered behaviour percentage
perDeadFields <- rep(0, ncol(dataMatrix))       # Dead fields percentage

# Iterate through each column (degeneration percentage)
for (col in 1:ncol(dataMatrix)) {
  # Get the data for the current column
  columnData <- dataMatrix[, col]
  
  # Count NaNs and valid data
  isDead <- sum(is.na(columnData))
  validData <- columnData[!is.na(columnData)]
  
  # Count dead fields and failed behaviour
  numDeadFields[col] <- isDead
  numFailedBehaviour[col] <- sum(validData > 0) + numDeadFields[col]
  
  # Recovered behaviour and correct behaviour
  numRecoveredBehaviour[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  numCorrectBehaviour[col] <- nrow(dataMatrix) - numFailedBehaviour[col]
  
  # Average relearning cycles
  avgRelearningCycles[col] <- mean(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
  # Percentages
  perCorrectBehaviour[col] <- (numCorrectBehaviour[col] / nrow(dataMatrix)) * 100
  perRecoveredBehaviour[col] <- (numRecoveredBehaviour[col] / numFailedBehaviour[col]) * 100
  perFailedBehaviour[col] <- (numFailedBehaviour[col] / nrow(dataMatrix)) * 100
  perDeadFields[col] <- (numDeadFields[col] / nrow(dataMatrix)) * 100
}

# Print experiment details
cat(paste0("Degeneracy type: ", degeneracyType, "\n"))
cat(paste0("Training dataset: ", relearningType, "\n"))
cat(paste0("Update all weights: ", updateAllWeights, "\n"))
cat(paste0("Epochs: ", epochs, "\n"))
cat(paste0("Max. demonstrations: ", maximumLearningCycles, "\n"))
cat(paste0("Number of trials: ", nrow(dataMatrix), "\n"))
cat("-----------------------------------------------------------------------------------------------------------------------------\n")

# Create a data frame for results
dataTable <- data.frame(
  `Deg. %` = degenerationPercentages,
  `Correct behaviour %` = perCorrectBehaviour,
  `Failed behaviour %` = perFailedBehaviour,
  `Recovered behaviour %` = perRecoveredBehaviour,
  `Avg. relearning cycles` = avgRelearningCycles,
  `"Dead" fields %` = perDeadFields
)

# Display the table
print(dataTable)

# Plot the evolution of failed behavior, recovered behavior, and average relearning cycles
relearning_scalar <- 20
y_axis_scale <- 20
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
    limits = c(10, 20),  # Set limits for the x-axis
    breaks = seq(10, 20, by = 1)  # Set ticks at 0.5 intervals
  ) +
  
  # Color and theme adjustments
  scale_color_manual(values = c('Failed Behavior' = 'red', 
                                'Recovered Behavior' = 'darkgreen', 
                                'Average Relearning Cycles' = 'blue')) +
  labs(
    # title = 'Behavior Evolution vs. Degeneration Percentage',
    # subtitle = 'Visualization of Failed and Recovered Behavior with Average Relearning Cycles',
    # caption = 'Data represents the evolution of behavior across different degeneration percentages.',
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
    text = element_text(family = "Garamond", size = 14),
    panel.grid.major = element_line(color = "lightgray", size = 0.5),
    panel.grid.minor = element_blank(),  # Remove minor grid lines for a cleaner look
    plot.title = element_text(face = "bold", size = 16),
    plot.subtitle = element_text(size = 12, face = "italic"),
    plot.caption = element_text(size = 10, face = "italic", color = "darkgray")
  )

# Save the plot
filename <- 'Relearning_deactivate_pre_synaptic_neurons'
ggsave(paste0('./plots/', filename, '.png'), width = 12, height = 8, dpi = 300)
