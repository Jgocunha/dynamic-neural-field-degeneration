library(rstudioapi)

# Defining the experiments as a data frame
experiments <- data.frame(
  experiment_name = c('deactivate weights', 'reduce 0.005 weights', 'randomize weights', 
                      'deactivate pre-synaptic neurons', 'deactivate post-synaptic neurons'),
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

current_dir <- dirname(rstudioapi::getActiveDocumentContext()$path)

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


# Create a loop to run the analysis for each experiment
for (experiment in 1:nrow(experiments)) {
  totalNumTrials <- 0  # Initialize total number of trials
  dataTable <- data.frame()  # Create an empty data frame
  
  # Construct analysis file path
  analysisFilePath <- paste0('./analysis/', experiments$experiment_name[experiment], ' - analysis.txt')
  
  # Loop over each position
  for (position in 1:length(positions)) {
    
    # Variable setup: Construct centroids file path
    centroidsFilePath <- paste0('../', positions[position], ' ', experiments$experiment_name[experiment], ' - centroids.txt')
    
    # Get target centroid for the current position
    targetCentroid <- targetCentroids[position]
    
    # Read and clean data
    data <- read_data(centroidsFilePath)
    data <- clean_data(data)
  }
}