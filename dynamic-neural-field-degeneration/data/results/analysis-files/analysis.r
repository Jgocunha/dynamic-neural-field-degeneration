library(rstudioapi)

# Defining the experiments as a data frame
experiments <- data.frame(
  experiment_name = c(
    'deactivate weights', 
    'reduce 0.005 weights', 
    'randomize weights', 
    'deactivate pre-synaptic neurons', 
    'deactivate post-synaptic neurons'
    ),
  variable = c(
    'weight', 
    'weight', 
    'weight', 
    'pre-synaptic neuron', 
    'post synaptic neuron'),
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

get_iterations_trial <- function(data) {
  # Determine the number of trials
  numTrials <- length(data)
  
  # Initialize a vector to store the number of iterations per trial
  numIterationsPerTrial <- numeric(numTrials)
  
  # Iterate over each trial
  for (trial in 1:numTrials) {
    # Get the number of iterations for the current trial
    numIterations <- length(data[[trial]])
    
    # Store the number of iterations for the current trial
    numIterationsPerTrial[trial] <- numIterations
  }
  
  # Calculate the average number of iterations
  avgNumIterations <- mean(numIterationsPerTrial)
  
  # Return both the number of iterations per trial and the average number of iterations
  return(list(numIterationsPerTrial = numIterationsPerTrial, avgNumIterations = avgNumIterations))
}

get_iterations_misbehavior <- function(data, targetCentroid, acceptableDeviation) {
  # Determine the number of trials
  numTrials <- length(data)
  
  # Initialize a vector to store the iteration indices where misbehavior occurs
  aboveOrBelowThreshold <- numeric(numTrials)
  
  # Iterate over each trial
  for (trial in 1:numTrials) {
    # Get the actual number of iterations for the current trial
    numIterations <- length(data[[trial]])
    
    # Iterate over each iteration within the current trial
    for (iteration in 1:numIterations) {
      # Calculate the deviation from the target centroid
      deviation <- min(abs(data[[trial]][iteration] - targetCentroid),
                       abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
      
      # Check if the deviation exceeds the acceptable deviation
      if (deviation >= acceptableDeviation) {
        aboveOrBelowThreshold[trial] <- iteration
        break  # Exit the loop if the condition is met
      } else {
        # If no deviation is found, record the last iteration
        aboveOrBelowThreshold[trial] <- iteration
      }
    }
  }
  
  # Calculate the average number of iterations until misbehavior
  avgIterations <- mean(aboveOrBelowThreshold)
  
  return(list(aboveOrBelowThreshold = aboveOrBelowThreshold, avgIterations = avgIterations))
}

get_max_deviations <- function(data, targetCentroid) {
  # Determine the number of trials
  numTrials <- length(data)
  
  # Initialize a vector to store the maximum deviations for each trial
  trialDeviations <- numeric(numTrials)
  
  # Iterate over each trial
  for (trial in 1:numTrials) {
    # Get the number of iterations for the current trial
    numIterations <- length(data[[trial]])
    
    # Initialize the maximum deviation for the current trial
    maxDeviation <- 0
    
    # Iterate over each iteration within the current trial
    for (iteration in 1:numIterations) {
      # Calculate the deviation from the target centroid
      deviation <- min(abs(data[[trial]][iteration] - targetCentroid),
                       abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
      
      # Update the maximum deviation if the current deviation is higher
      if (deviation > maxDeviation) {
        maxDeviation <- deviation
      }
    }
    
    # Store the maximum deviation for the current trial
    trialDeviations[trial] <- maxDeviation
  }
  
  # Find the overall maximum deviation across all trials
  maxDeviations <- max(trialDeviations)
  
  return(list(trialDeviations = trialDeviations, maxDeviations = maxDeviations))
}

get_avg_centroid <- function(data) {
  # Determine the number of trials
  numTrials <- length(data)
  
  # Initialize a vector to store the average centroid value for each trial
  avgCentroidValuePerTrial <- numeric(numTrials)
  
  # Iterate over each trial
  for (trial in 1:numTrials) {
    # Calculate and store the average centroid value for the current trial
    avgCentroidValuePerTrial[trial] <- mean(data[[trial]])
  }
  
  # Calculate the overall average centroid value across all trials
  avgCentroidValue <- mean(avgCentroidValuePerTrial)
  
  return(list(avgCentroidValuePerTrial = avgCentroidValuePerTrial, avgCentroidValue = avgCentroidValue))
}

adapt_to_percentage <- function(experiment, avgNumIterations, avgIterationsMisbehavior) {
  # Set the size based on the experiment type
  if (experiment == 'deactivate weights') {
    size <- 202
  } else if (experiment == 'reduce 0.005 weights') {
    size <- 202
  } else if (experiment == 'randomize weights') {
    size <- 202
  } else if (experiment == 'deactivate pre-synaptic neurons') {
    size <- 720
  } else if (experiment == 'deactivate post-synaptic neurons') {
    size <- 280
  } else {
    stop("Unknown experiment type")
  }
  
  # Calculate percentages
  avgNumIterationsPercentage <- (avgNumIterations * 100) / size
  avgIterationsMisbehaviorPercentage <- (avgIterationsMisbehavior * 100) / size
  
  return(list(avgNumIterationsPercentage = avgNumIterationsPercentage, 
              avgIterationsMisbehaviorPercentage = avgIterationsMisbehaviorPercentage))
}

# Create a loop to run the analysis for each experiment
for (experiment in 1:nrow(experiments)) {
  totalNumTrials <- 0
  
  # Initialize an empty data frame to store results outside the inner loop
  dataTable <- data.frame(
    Condition = character(),
    Target_centroid = numeric(),
    Trials = integer(),
    Avg_percent_affected_elements_until_disappearance = numeric(),
    Avg_percent_affected_elements_until_misbehavior = numeric(),
    Max_deviation = numeric(),
    stringsAsFactors = FALSE
  )
  
  for (position in 1:length(positions)) {
    # Correctly format the position as a string with one decimal place
    positionStr <- sprintf("%.1f", positions[position])
    
    # Construct the correct file path
    centroidsFilePath <- paste0('../', positionStr, ' ', experiments$experiment_name[experiment], ' - centroids.txt')
    
    targetCentroid <- targetCentroids[position]
    
    # Read and clean data
    data <- read_data(centroidsFilePath)
    data <- clean_data(data)
    
    # Analyse data
    # Get number of trials
    numTrials <- length(data)
    
    # Get iterations until disappearance of peak
    iterationResults <- get_iterations_trial(data)
    avgNumIterations <- iterationResults$avgNumIterations
    
    # Get iterations until misbehavior of peak
    misbehaviorResults <- get_iterations_misbehavior(data, targetCentroid, acceptableDeviation)
    avgIterationsMisbehavior <- misbehaviorResults$avgIterations
    
    # Get deviations and maximum deviation
    deviationResults <- get_max_deviations(data, targetCentroid)
    maxDeviations <- deviationResults$maxDeviations
    
    # Adapt values to percentages
    percentageResults <- adapt_to_percentage(experiments$experiment_name[experiment], avgNumIterations, avgIterationsMisbehavior)
    avgNumIterationsPercentage <- percentageResults$avgNumIterationsPercentage
    avgIterationsMisbehaviorPercentage <- percentageResults$avgIterationsMisbehaviorPercentage
    
    # Store data in table
    newRow <- data.frame(
      Condition = experiments$experiment_name[experiment],
      Target_centroid = targetCentroid,
      Trials = numTrials,
      Avg_percent_affected_elements_until_disappearance = avgNumIterationsPercentage,
      Avg_percent_affected_elements_until_misbehavior = avgIterationsMisbehaviorPercentage,
      Max_deviation = maxDeviations,
      stringsAsFactors = FALSE
    )
    
    # Append newRow to dataTable
    dataTable <- rbind(dataTable, newRow)
    
    # Increment the total number of trials for this experiment
    totalNumTrials <- totalNumTrials + numTrials
  }
  # Calculate and display Avg. % of affected elements until disappearance of bump
  avgAvgNumIterations <- mean(dataTable$Avg_percent_affected_elements_until_disappearance)
  stdAvgNumIterations <- sd(dataTable$Avg_percent_affected_elements_until_disappearance)
  seAvgNumIterations <- stdAvgNumIterations / sqrt(nrow(dataTable))
  print(paste('Avg. % of affected elements until disappearance of bump: ', round(avgAvgNumIterations, 4)))
  print(paste('Standard Error: ', round(seAvgNumIterations, 4)))
  
  # Calculate and display average Avg. % of affected elements until misbehaviour
  avgAvgIterationsMisbehavior <- mean(dataTable$Avg_percent_affected_elements_until_misbehavior)
  stdAvgIterationsMisbehavior <- sd(dataTable$Avg_percent_affected_elements_until_misbehavior)
  seAvgIterationsMisbehavior <- stdAvgIterationsMisbehavior / sqrt(nrow(dataTable))
  print(paste('Avg. % of affected elements until misbehavior: ', round(avgAvgIterationsMisbehavior, 4)))
  print(paste('Standard Error: ', round(seAvgIterationsMisbehavior, 4)))
  
  # Calculate and display average Max. deviation per experiment
  avgMaxDeviation <- mean(dataTable$Max_deviation)
  stdMaxDeviation <- sd(dataTable$Max_deviation)
  seMaxDeviation <- stdMaxDeviation / sqrt(nrow(dataTable))
  print(paste('Average Max. deviation per experiment: ', round(avgMaxDeviation, 4)))
  print(paste('Standard Error: ', round(seMaxDeviation, 4)))
  
  print(dataTable)
  
  # Construct results: Add summary statistics first
  results_summary <- data.frame(
    Metric = c(
      'Avg. % of affected elements until disappearance of bump',
      'Standard Error (disappearance)',
      'Avg. % of affected elements until misbehavior',
      'Standard Error (misbehavior)',
      'Average Max. deviation per experiment',
      'Standard Error (max deviation)'
    ),
    Value = c(
      round(avgAvgNumIterations, 4),
      round(seAvgNumIterations, 4),
      round(avgAvgIterationsMisbehavior, 4),
      round(seAvgIterationsMisbehavior, 4),
      round(avgMaxDeviation, 4),
      round(seMaxDeviation, 4)
    ),
    stringsAsFactors = FALSE
  )
  
  # Create the directory if it doesn't exist
  if (!dir.exists("./analysis/")) {
    dir.create("./analysis/")
  }
  
  # Construct the filename for the results table
  results_filename <- paste0("./analysis/", experiments$experiment_name[experiment], ' - analysis.txt')
  
  # Write the data table (detailed trial results) by appending it to the file
  write.table(
    dataTable,               # The data frame to write (dataTable)
    file = results_filename, # The same file name
    sep = "\t",              # Use tab as the separator
    row.names = FALSE,       # Do not include row names
    col.names = TRUE,        # Include column names
    quote = TRUE,            # Quote character strings
    append = FALSE            # Do not Append to the existing file
  )
  
  # Write a separator to the file (a blank line or header for the next section)
  write("\nSummary Statistics:\n", file = results_filename, append = TRUE)
  
  # Write summary statistics to the file first
  write.table(
    results_summary,         # The summary statistics
    file = results_filename, # The output file name
    sep = "\t",              # Use tab as the separator
    row.names = FALSE,       # Do not include row names
    col.names = FALSE,        # Do not Include column names
    quote = TRUE,             # Quote character strings
    append = TRUE            # Append to the existing file
  )
  
}
