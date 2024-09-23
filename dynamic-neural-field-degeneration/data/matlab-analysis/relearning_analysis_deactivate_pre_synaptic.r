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
plot_data <- data.frame(
  Degeneration = degenerationPercentages,
  Failed = perFailedBehaviour,
  Recovered = perRecoveredBehaviour,
  AvgRelearningCycles = avgRelearningCycles
)

ggplot(plot_data) +
  geom_line(aes(x = Degeneration, y = Failed, color = 'red')) +
  geom_point(aes(x = Degeneration, y = Failed), color = 'red') +
  geom_line(aes(x = Degeneration, y = Recovered, color = 'darkgreen')) +
  geom_point(aes(x = Degeneration, y = Recovered), color = 'darkgreen') +
  geom_line(aes(x = Degeneration, y = AvgRelearningCycles, color = 'blue'), linetype = "dashed") +
  geom_point(aes(x = Degeneration, y = AvgRelearningCycles), color = 'blue') +
  scale_y_continuous(sec.axis = sec_axis(~., name = "Average Relearning Cycles")) +
  labs(title = 'Evolution of Behavior and Relearning Cycles', x = 'Degeneration Percentage', 
       y = 'Recovered Behavior % / Failed Behavior %') +
  scale_color_manual(name = "Legend", values = c('red' = 'red', 'darkgreen' = 'darkgreen', 'blue' = 'blue')) +
  theme_minimal()

# Save the plot
#filename <- 'Relearning_deactivate_pre_synaptic_neurons'
#ggsave(paste0('./plots/', filename, '.png'), width = 12, height = 8, dpi = 300)
