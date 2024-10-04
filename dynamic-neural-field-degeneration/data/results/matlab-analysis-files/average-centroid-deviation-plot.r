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

get_centroid_deviations <- function(data, targetCentroid) {
  deviations <- matrix(0, nrow = length(data), ncol = max(sapply(data, length)))
  
  for (trial in 1:length(data)) {
    for (iteration in 1:length(data[[trial]])) {
      deviations[trial, iteration] <- min(abs(data[[trial]][iteration] - targetCentroid), 
                                          abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
    }
  }
  return(deviations)
}

plot_centroid_avg_deviation <- function(deviations, locationDirPath, figTitle, subTitle, 
                                        targetCentroid, acceptableDeviation, numberOfElementsTotal, 
                                        numberOfElementsDegeneratedPerIteration) {
  
  stdLine <- apply(deviations, 2, sd, na.rm = TRUE)
  avgLine <- apply(deviations, 2, mean, na.rm = TRUE)
  
  maxSize <- ncol(deviations)
  maxSize <- (maxSize * 100 / numberOfElementsTotal) * numberOfElementsDegeneratedPerIteration
  
  percentageValues <- seq(0, maxSize, length.out = length(avgLine))
  
  # Plot std deviation and average deviation
  p <- ggplot() +
    geom_line(aes(x = percentageValues, y = stdLine), color = 'blue', size = 1.5) +
    geom_line(aes(x = percentageValues, y = avgLine), color = 'red', size = 1.5) +
    labs(title = paste(figTitle, "\n", subTitle),
         x = "Degeneration percentage",
         y = "Deviation",
         subtitle = "Average and Standard Deviation") +
    theme_minimal() +
    theme(text = element_text(size = 12, family = "Garamond"))
  
  # Save the plot
  filename <- gsub('[^[:alnum:]]', '', paste(figTitle, subTitle))
  ggsave(filename = paste0(locationDirPath, filename, ".png"), plot = p, width = 10, height = 6)
}


# Loop through each experiment
for (experiment in 1:nrow(experiments)) {
  
  # Define the title based on experiment
  experimentTitle <- switch(experiments$experiment_name[experiment],
                            'deactivate pre-synaptic neurons' = paste("Deactivating one unique", experiments$variable[experiment]),
                            'deactivate post-synaptic neurons' = paste("Deactivating one unique", experiments$variable[experiment]),
                            'reduce 0.05 weights' = paste("Randomly reducing one", experiments$variable[experiment], "by 95%"),
                            'randomize weights' = paste("Randomly randomizing one", experiments$variable[experiment]),
                            'deactivate weights' = paste("Randomly deactivating one unique", experiments$variable[experiment])
  )
  
  # Loop through positions
  for (positionIndex in 1:length(positions)) {
    positionStr <- sprintf("%.1f", positions[positionIndex])
    targetCentroid <- targetCentroids[positionIndex]
    subTitle <- paste("Target centroid position", positionStr)
    
    # Load centroid data
    centroidsFilePath <- paste0('../', positionStr, ' ', experiments$experiment_name[experiment], ' - centroids.txt')
    data <- read_data(centroidsFilePath)
    data <- clean_data(data)
    
    # Get centroid deviations
    deviations <- get_centroid_deviations(data, targetCentroid)
    
    # Define parameters for the plot
    numberOfElementsTotal <- 100  # Example value
    numberOfElementsDegeneratedPerIteration <- 1  # Example value
    
    # Generate and save the plot
    plot_centroid_avg_deviation(deviations, plotFilePath, experimentTitle, subTitle, 
                                targetCentroid, acceptableDeviation, 
                                numberOfElementsTotal, numberOfElementsDegeneratedPerIteration)
  }
}