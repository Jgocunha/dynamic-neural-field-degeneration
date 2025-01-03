library(rstudioapi)
library(ggplot2)
library(dplyr)
library(tidyr)
library(extrafont)
library(gganimate)
library(gifski)

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
    'Post-synaptic neuron'
  ),
  deg_per = c(
    0.496,
    0.496,
    0.496,
    0.1389,
    0.3571
  ),
  stringsAsFactors = FALSE
)

# Positions and target centroids
positions <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)
targetCentroids <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)

# File paths
centroidsFilePath <- ''
plotFilePath <- './plots/max-abs-dev-N-trials/'

# Loop through each position and create a corresponding directory
for (pos in positions) {
  # Create a directory name using the position
  dir_name <- paste0(pos)
  
  # Full path for the new directory
  full_path <- file.path(plotFilePath, dir_name)
  
  # Create the directory
  dir.create(full_path, showWarnings = FALSE) # showWarnings = FALSE suppresses warnings if the directory already exists
}

# Acceptable deviation
acceptableDeviation <- 2.0

# Number of random trials to select
X <- 5  # You can change X to select the number of random trials

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

get_centroid_deviations <- function(data, targetCentroid) {
  for (trial in 1:length(data)) {
    deviations <- numeric(length(data[[trial]]))
    
    for (iteration in 1:length(data[[trial]])) {
      deviations[iteration] <- min(abs(data[[trial]][iteration] - targetCentroid), 
                                   abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
    }
  }
  return(deviations)
}

plot_and_animate <- function(deviations, targetCentroid, positionStr, experimentNameStr, trialsToPlot, experimentTitle, deg_per) {
  # Create a data frame for ggplot with degeneration percentages
  deviation_df <- do.call(rbind, lapply(1:length(deviations), function(trialIndex) {
    trialID <- trialsToPlot[trialIndex]  # Get the actual trial identifier
    iterations <- 1:length(deviations[[trialIndex]])
    deg_per_x <- iterations * deg_per  # Convert iterations to degeneration percentages
    data.frame(
      trial = as.factor(trialID),  # Use the actual trial ID for the legend
      degeneration_percentage = deg_per_x,  # Use degeneration percentage for the x-axis
      deviation = deviations[[trialIndex]]
    )
  }))
  
  # Title and Subtitle
  Title <- paste0("Max. Abs. Deviation of Centroid as Degeneration Progresses for ", 
                  length(trialsToPlot), " Randomly Selected Trials")
  SubTitle <- paste0(experimentTitle, " - Target Centroid Position is ", positionStr)
  
  # Create the ggplot with degeneration percentages on x-axis
  p <- ggplot(deviation_df, aes(x = degeneration_percentage, y = deviation, color = trial)) +
    geom_line(size = 1.2) +  # Line thickness
    geom_point(size = 2) +
    scale_color_brewer(palette = "Paired", name = "Trial") +  # Use pastel palette from ColorBrewer    
    labs(
      title = Title,
      subtitle = SubTitle,
      x = "Degeneration Percentage (%)",  # Adjusted x-axis label
      y = "Absolute Deviation"
    ) +
    theme_classic(base_family = "Times New Roman") +
    theme(
      legend.position = "right",  # Place legend on the right
      text = element_text(size = 14, family = "Times New Roman"),
      plot.title = element_text(face = "bold", size = 16),  # Bold title
      axis.title = element_text(face = "bold"),  # Bold axis titles
      panel.grid.major = element_line(color = "lightgray", size = 0.5)
    ) +
    scale_x_continuous(breaks = seq(0, max(deviation_df$degeneration_percentage) * 1.05, by = deg_per * 10), 
                       expand = c(0, 0), limits = c(0, max(deviation_df$degeneration_percentage) * 1.05)) +  # Adjust x-axis for degeneration percentage
    scale_y_continuous(expand = c(0, 0), limits = c(0, max(deviation_df$deviation) * 1.05)) +
    transition_reveal(degeneration_percentage)
  
  # Create the animation
  animation <- animate(p, fps = 10, width = 800, height = 600, dpi = 300)
  animation_to_save <- animation + 
    enter_grow() + 
    exit_fade() +
    exit_shrink()
  
  # Check if animation is a valid object
  print(animation_to_save)
  
  # Save the animation as .gif
  anim_save(paste0(plotFilePath, "/", targetCentroid, "/", experimentNameStr, ".gif"), 
            animation = animation)
  
  # Save the animation as .svg frames
  animate(p, nframes = 2, device = "svg",
          renderer = file_renderer(paste0(plotFilePath, "/", targetCentroid, "/"), 
                                   prefix = paste0(experimentNameStr, "frame"), 
                                   overwrite = TRUE))
}

# Loop through each experiment
for (experiment in 1:nrow(experiments)) {
  
  # Define the title based on experiment
  experimentTitle <- switch(experiments$experiment_name[experiment],
                            'deactivate pre-synaptic neurons' = paste("Deactivating One Unique", experiments$variable[experiment]),
                            'deactivate post-synaptic neurons' = paste("Deactivating One Unique", experiments$variable[experiment]),
                            'reduce 0.05 weights' = paste("Randomly Reducing One", experiments$variable[experiment], "by 95%"),
                            'randomize weights' = paste("Randomly Randomizing One", experiments$variable[experiment]),
                            'deactivate weights' = paste("Randomly Deactivating One Unique", experiments$variable[experiment])
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
    
    # Randomly select X trials
    trialsToPlot <- sample(1:length(data), X)
    deviations <- list()
    
    # Store deviations for each selected trial
    for (trial in trialsToPlot) {
      # Max. Abs. Dev.
      deviations <- append(deviations, list(get_max_centroid_deviations(data[trial], targetCentroid)))
      # Abs. Dev.
      #deviations <- append(deviations, list(get_centroid_deviations(data[trial], targetCentroid)))
    }
    
    # Plot and animate the selected trials
    p <- plot_and_animate(
      deviations, 
      targetCentroid, 
      positionStr, 
      experiments$experiment_name[experiment], 
      trialsToPlot,
      experimentTitle,
      experiments$deg_per[experiment] 
      )
  }
}
