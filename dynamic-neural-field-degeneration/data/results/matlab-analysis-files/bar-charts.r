library(rstudioapi)
library(ggplot2)
library(dplyr)
library(tidyr)

# Defining the experiments as a data frame
experiments <- data.frame(
  experiment_name = c(#'deactivate weights', 
                      #'reduce 0.005 weights', 
                      #'randomize weights', 
                      'deactivate pre-synaptic neurons'), 
                      #'deactivate post-synaptic neurons'),
  variable = c('weight', 'weight', 'weight', 'pre-synaptic neuron', 'post synaptic neuron'),
  stringsAsFactors = FALSE
)

# Positions and target centroids
positions <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)
targetCentroids <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)

# File paths
centroidsFilePath <- ''
analysisFilePath <- ''
plotFilePath <- './plots/'

# Acceptable deviation
acceptableDeviation <- 2.0
degenerationInterval <- 1  # Interval to analyze columns in steps of 20

current_dir <- dirname(rstudioapi::getActiveDocumentContext()$path)

# Helper functions
read_data <- function(centroidFilePath) {
  # Open the file and read it line by line
  lines <- readLines(centroidFilePath)
  
  # Initialize an empty list to store the filtered lines
  data <- list()
  
  for (line in lines) {
    # Split the line into components and convert to numeric values
    lineData <- as.numeric(unlist(strsplit(line, "\\s+")))
    
    # Remove NA and values equal to 0
    lineData <- lineData[!is.na(lineData) & lineData != 0]
    
    # If the line has data, add it to the data list
    if (length(lineData) > 0) {
      data <- append(data, list(lineData))
    }
  }
  
  return(data)
}

clean_data <- function(data) {
  # Initialize an empty list to store filtered values
  newData <- list()
  
  # Loop through each element in the data list
  for (i in 1:length(data)) {
    if (is.numeric(data[[i]])) {
      # Keep only non-negative numeric values
      if (all(data[[i]] >= 0)) {
        newData <- append(newData, list(data[[i]]))
      }
    } else if (is.logical(data[[i]]) || is.character(data[[i]])) {
      # Keep logical and character values as they are
      newData <- append(newData, list(data[[i]]))
    }
  }
  
  return(newData)  # Return the cleaned data
}

# Create loop to analyze each experiment
for (experiment in 1:nrow(experiments)) {
  totalNumTrials <- 0
  
  # Initialize a data frame to store results
  dataTable <- data.frame(
    Condition = character(),
    Target_centroid = numeric(),
    Trials = integer(),
    stringsAsFactors = FALSE
  )
  
  for (positionIndex in 1:length(positions)) {
    positionStr <- sprintf("%.1f", positions[positionIndex])
    targetCentroid <- targetCentroids[positionIndex]
    
    # Construct file path for the current experiment and position
    centroidsFilePath <- paste0('../', positionStr, ' ', experiments$experiment_name[experiment], ' - centroids.txt')
    
    # Read and clean data
    data <- read_data(centroidsFilePath)
    #data <- clean_data(data)
    
    # Prepare to count behavior types at each degeneration step
    degenerationSteps <- seq(1, length(data[[1]]), by = degenerationInterval)  # Analyze in steps of 20 columns
    
    correctCount <- rep(0, length(degenerationSteps))
    failedCount <- rep(0, length(degenerationSteps))
    misbehaviorCount <- rep(0, length(degenerationSteps))
    
    # Analyze each degeneration step (each column, stepping through degenerationInterval)
    for (degenerationIndex in seq_along(degenerationSteps)) {
      columnIndex <- degenerationSteps[degenerationIndex]
      
      
      # Analyze each row (trial)
      for (trial in 1:length(data)) {
        if (length(data[[trial]]) < columnIndex) {
          # If there's no value in this degeneration step (failed behavior)
          failedCount[degenerationIndex] <- failedCount[degenerationIndex] + 1
        } else {
          # Get the centroid value at this degeneration step
          currentValue <- data[[trial]][columnIndex]
          
          # Determine acceptable range
          lowerBound <- targetCentroid - acceptableDeviation
          upperBound <- targetCentroid + acceptableDeviation
          
          # Check behavior type
          if (currentValue >= lowerBound && currentValue <= upperBound) {
            correctCount[degenerationIndex] <- correctCount[degenerationIndex] + 1
          } else {
            misbehaviorCount[degenerationIndex] <- misbehaviorCount[degenerationIndex] + 1
          }
        }
      }
    }
    
    # Create data frame for plotting
    results <- data.frame(
      Degeneration_Step = degenerationSteps,
      Correct = correctCount,
      Failed = failedCount,
      Misbehavior = misbehaviorCount
    )
    
    # Convert to long format for ggplot
    results_long <- pivot_longer(results, cols = c(Correct, Failed, Misbehavior), names_to = "Behavior", values_to = "Count")
    
    # Plot the results for this position and experiment
    p <- ggplot(results_long, aes(x = factor(Degeneration_Step), y = Count, fill = Behavior)) +
      geom_bar(stat = "identity", position = "dodge") +
      labs(title = paste("Behavior analysis for position", positionStr, "and experiment", experiments$experiment_name[experiment]),
           x = "Degeneration Step",
           y = "Count",
           fill = "Behavior Type") +
      theme_minimal() +
      theme(axis.text.x = element_text(angle = 45, hjust = 1))
    
    # Save the plot
    ggsave(filename = paste0(plotFilePath, "plot_", positionStr, "_", experiments$experiment_name[experiment], ".png"),
           plot = p, width = 10, height = 6)
  }
}
