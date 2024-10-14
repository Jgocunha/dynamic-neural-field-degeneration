library(rstudioapi)
library(ggplot2)
library(dplyr)
library(extrafont)

#loadfonts(device = "win")

# Get the script directory
current_dir <- dirname(rstudioapi::getActiveDocumentContext()$path)

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
    'Weight', 
    'Weight', 
    'Weight', 
    'Pre-synaptic neuron', 
    'Post synaptic neuron'),
  stringsAsFactors = FALSE
)
# Positions and target centroids
positions <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)
targetCentroids <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)

# File paths
centroidsFilePath <- ''
plotFilePath <- './plots/avg-max-abs-dev/'

# Initialize an empty list to store results for each experiment
results <- list()

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

get_max_centroid_deviations <- function(data, targetCentroid) {
  max_deviations <- numeric(length(data))
  max_value <- 0.0
  
  for (trial in 1:length(data)) {
    deviations <- numeric(length(data[[trial]]))
    
    for (iteration in 1:length(data[[trial]])) {
      deviations[iteration] <- min(abs(data[[trial]][iteration] - targetCentroid), 
                                   abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
    }
    
    for (iteration in 1:length(data[[trial]])) {
      if (deviations[iteration] >= max_value) {
        max_value <- deviations[iteration]
      }
      max_deviations[iteration] <- max_value
    }
  }
  
  return(max_deviations)
}


# Loop through each experiment
for (experiment in 1:nrow(experiments)) {
  
  # Initialize a list to store deviations for each position
  position_deviations_list <- vector("list", length(positions))
  
  # Loop through positions
  for (positionIndex in 1:length(positions)) {
    positionStr <- sprintf("%.1f", positions[positionIndex])
    targetCentroid <- targetCentroids[positionIndex]
    
    # Load centroid data
    centroidsFilePath <- paste0('../', positionStr, ' ', experiments$experiment_name[experiment], ' - centroids.txt')
    data <- read_data(centroidsFilePath)
    data <- clean_data(data)
    
    # Calculate maximum centroid deviations for the current target centroid
    max_deviations <- get_max_centroid_deviations(data, targetCentroid)
    
    # Store max deviations for the current position
    position_deviations_list[[positionIndex]] <- max_deviations
  }
  
  # Calculate the average max deviation for each position
  avg_deviations_per_position <- sapply(position_deviations_list, function(x) mean(x, na.rm = TRUE))
  
  # Store results with experiment name and average max deviations per position
  results[[experiment]] <- data.frame(
    experiment_name = experiments$experiment_name[experiment],
    position = positions,
    avg_max_deviation = avg_deviations_per_position
  )
}


# Combine all results into a single data frame
final_results <- do.call(rbind, results)

Title <- "Max. Abs. Deviation of Centroid as Degeneration Progresses Averaged Across All Trials"
#SubTitle <- paste0()
  
# Create a ggplot
p <- ggplot(final_results, aes(x = factor(position), y = avg_max_deviation, color = experiment_name)) +
  geom_line(aes(group = experiment_name), size = 1) +
  geom_point(size = 3) +
  labs(title = Title,
       x = "Target Position",
       y = "Average Maximum Deviation",
       color = "Experiment") +
  theme_minimal() +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))

print(p)