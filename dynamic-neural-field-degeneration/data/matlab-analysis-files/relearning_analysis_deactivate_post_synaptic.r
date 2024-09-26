# Clear environment and graphics
rm(list = ls())
graphics.off()

# Load required libraries
library(ggplot2)

# Experiment parameters
resultPath <- '../results/'
degeneracyType <- 'deactivate post-synaptic neurons'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 50
updateAllWeights <- 0

filePath <- paste0(resultPath, degeneracyType, ' ', relearningType, '  Epochs-', 
                   epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                   ' Update-all-weights-', updateAllWeights, '.txt')


# Define the function to read data from the file
read_data <- function(filePath) {
  # Read the entire file as lines
  lines <- readLines(filePath)
  
  # Split each line by spaces or tabs (depending on your file format)
  split_lines <- strsplit(lines, " ")  # Replace " " with "\t" for tab-separated data
  
  # Find the maximum number of columns across all rows
  maxColumns <- max(sapply(split_lines, length))
  
  # Ensure all rows have the same length by padding shorter ones with NA
  padded_lines <- lapply(split_lines, function(x) {
    length(x) <- maxColumns  # Pad with NAs
    return(x)
  })
  
  # Convert the list of padded lines into a matrix
  dataMatrix <- do.call(rbind, padded_lines)
  
  # Convert the character matrix to numeric if necessary
  dataMatrix <- apply(dataMatrix, 2, as.numeric)
  
  return(list(dataMatrix = dataMatrix, maxColumns = maxColumns))
}

# Read data from file
data <- read_data(filePath)
dataMatrix <- data$dataMatrix
maxColumns <- data$maxColumns

# Initialize variables to store results
initialPer <- 80
incPer <- 0.36
finalPer <- initialPer + (incPer * maxColumns) - incPer
degenerationPercentages <- seq(initialPer, finalPer, by = incPer)

numCorrectBehaviour <- rep(0, ncol(dataMatrix))
numFailedBehaviour <- rep(0, ncol(dataMatrix))
numRecoveredBehaviour <- rep(0, ncol(dataMatrix))
numDeadFields <- rep(0, ncol(dataMatrix))
isDead <- 0

avgRelearningCycles <- rep(0, ncol(dataMatrix))

perCorrectBehaviour <- rep(0, ncol(dataMatrix))
perFailedBehaviour <- rep(0, ncol(dataMatrix))
perRecoveredBehaviour <- rep(0, ncol(dataMatrix))
perDeadFields <- rep(0, ncol(dataMatrix))

# Iterate through each column (degeneration percentage)
for (col in 1:ncol(dataMatrix)) {
  # Get the data for the current column
  columnData <- dataMatrix[, col]
  
  # Check for NaN
  isDead <- sum(is.na(columnData))

  # Filter out zeros that appear after NaN
  validData <- columnData[!is.na(columnData)]

  # Count the number of "dead" fields
  numDeadFields[col] <- isDead
  
  # Count the number of times it failed to exhibit the correct behaviour
  numFailedBehaviour[col] <- sum(validData > 0) + numDeadFields[col]
  
  # Count the number of times relearned (value > 0)
  numRecoveredBehaviour[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  
  # Count the number of fields that exhibited correct behaviour
  numCorrectBehaviour[col] <- nrow(dataMatrix) - numFailedBehaviour[col]
  
  # Calculate the average relearning cycles needed
  avgRelearningCycles[col] <- mean(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
  # Calculate percentage of fields that exhibited correct behaviour
  perCorrectBehaviour[col] <- (numCorrectBehaviour[col] / nrow(dataMatrix)) * 100
  
  # Calculate the percentage of fields that recovered from degeneration
  perRecoveredBehaviour[col] <- (numRecoveredBehaviour[col] / numFailedBehaviour[col]) * 100
  
  # Calculate percentage of fields that failed behaviour
  perFailedBehaviour[col] <- (numFailedBehaviour[col] / nrow(dataMatrix)) * 100
  
  # Calculate percentage of dead fields
  perDeadFields[col] <- (numDeadFields[col] / nrow(dataMatrix)) * 100
}

# Create a data frame for plotting and displaying
dataTable <- data.frame(
  `Degeneration %` = degenerationPercentages,
  `Correct behaviour %` = perCorrectBehaviour,
  `Failed behaviour %` = perFailedBehaviour,
  `Recovered behaviour %` = perRecoveredBehaviour,
  `Avg. relearning cycles` = avgRelearningCycles,
  `Dead fields %` = perDeadFields
)

# Display the table
print(dataTable)

# Plot the evolution of failed behavior, recovered behavior, and average relearning cycles
relearning_scalar <- 25
y_axis_scale <- 25
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
    limits = c(85, 100),  # Set limits for the x-axis
    breaks = seq(85, 100, by = 5)  # Set ticks at 0.5 intervals
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
filename <- 'Relearning_post_synaptic_neurons'
ggsave(paste0('./plots/', filename, '.png'), width = 12, height = 8, dpi = 300)
