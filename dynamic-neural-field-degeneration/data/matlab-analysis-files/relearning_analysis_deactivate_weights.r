# Load required libraries
library(ggplot2)

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
initialPer <- 90
incPer <- 0.5
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
  avgRelearningCycles[col] <- mean(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  
  # Calculate percentages
  perCorrectBehaviour[col] <- numCorrectBehaviour[col] / nrow(dataMatrix) * 100
  perRecoveredBehaviour[col] <- numRecoveredBehaviour[col] / numFailedBehaviour[col] * 100
  perFailedBehaviour[col] <- numFailedBehaviour[col] / nrow(dataMatrix) * 100
  perDeadFields[col] <- numDeadFields[col] / nrow(dataMatrix) * 100
}

# Display the summary information
cat('Degeneracy type: ', degeneracyType, '\n')
cat('Training dataset: ', relearningType, '\n')
cat('Update all weights: ', updateAllWeights, '\n')
cat('Epochs: ', epochs, '\n')
cat('Max. demonstrations: ', maximumLearningCycles, '\n')
cat('Number of trials: ', nrow(dataMatrix), '\n')
cat('-----------------------------------------------------------------------------------------------------------------------------\n')

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
ggplot() +
  geom_line(aes(x = degenerationPercentages, y = perFailedBehaviour, color = 'Failed Behavior'), size = 1.5) +
  geom_point(aes(x = degenerationPercentages, y = perFailedBehaviour, color = 'Failed Behavior'), size = 3) +
  geom_line(aes(x = degenerationPercentages, y = perRecoveredBehaviour, color = 'Recovered Behavior'), size = 1.5) +
  geom_point(aes(x = degenerationPercentages, y = perRecoveredBehaviour, color = 'Recovered Behavior'), shape = 15, size = 3) +
  geom_line(aes(x = degenerationPercentages, y = avgRelearningCycles, color = 'Average Relearning Cycles'), size = 1.5) +
  geom_point(aes(x = degenerationPercentages, y = avgRelearningCycles, color = 'Average Relearning Cycles'), shape = 17, size = 3) +
  scale_color_manual(values = c('Failed Behavior' = 'red', 'Recovered Behavior' = 'darkgreen', 'Average Relearning Cycles' = 'blue')) +
  labs(title = 'Evolution of Behavior and Relearning Cycles',
       x = 'Degeneration Percentage',
       y = 'Percentage / Cycles',
       color = 'Legend') +
  theme_minimal(base_size = 15) +
  theme(legend.position = 'top')

# Save the plot as an image
#filename <- 'Relearning_deactivate_weights.png'
#ggsave(filename, width = 12, height = 8, dpi = 300)
