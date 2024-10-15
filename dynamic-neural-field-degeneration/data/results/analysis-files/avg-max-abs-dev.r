library(rstudioapi)
library(ggplot2)
library(dplyr)
library(tidyr)
library(plotrix)
library(extrafont)
library(gridExtra)
library(grid)

loadfonts(device = "win")

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
current_dir <- dirname(rstudioapi::getActiveDocumentContext()$path)
centroidsFilePath <- ''
plotFilePath <- './plots/avg-max-abs-dev/'

# Loop through each experiment name and create a corresponding directory
for (experiment in experiments$experiment_name) {
  # Create a directory name using the experiment name
  dir_name <- paste0(experiment)
  
  # Full path for the new directory
  full_path <- file.path(plotFilePath, dir_name)
  
  # Create the directory
  dir.create(full_path, showWarnings = FALSE) # showWarnings = FALSE suppresses warnings if the directory already exists
}

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
  # Initialize matrices for deviations and max_deviations
  deviations <- matrix(0, nrow = length(data), ncol = max(sapply(data, length)))
  max_deviations <- matrix(0, nrow = length(data), ncol = max(sapply(data, length)))
  
  # Loop over trials
  for (trial in 1:length(data)) {
    max_value <- 0.0  # Reset max value for each trial
    
    # Loop over iterations for each trial
    for (iteration in 1:length(data[[trial]])) {
      # Calculate the deviation for the current iteration
      deviation <- min(abs(data[[trial]][iteration] - targetCentroid), 
                       abs(28 - abs(data[[trial]][iteration] + targetCentroid)))
      deviations[trial, iteration] <- deviation
    }
  }
  
  return(deviations)
}

plot_centroid_avg_deviation <- function(deviations, locationDirPath, figTitle, subTitle, 
                                        targetCentroid, experimentName, deg_per) {
  # Calculate average and standard error
  avgLine <- apply(deviations, 2, mean, na.rm = TRUE)
  semLine <- apply(deviations, 2, std.error, na.rm = TRUE)
  iterations <- seq_along(avgLine)
  
  # Find the index of the maximum value in avgLine
  max_index <- which.max(avgLine)
  
  # Trim data to only include values up to the maximum y-value
  trimmed_iterations <- iterations[1:max_index]
  trimmed_avgLine <- avgLine[1:max_index]
  trimmed_semLine <- semLine[1:max_index]
  
  # Convert simulation iterations to degeneration percentages
  trimmed_deg_per <- iterations * deg_per
  
  # Create a data frame for plotting the trimmed data
  plot_data <- data.frame(
    DegenerationPercentage = trimmed_deg_per,
    Avg = avgLine,
    LowerBound = avgLine - semLine,
    UpperBound = avgLine + semLine
  )
  
  Title <- "Max. Abs. Deviation of Centroid as Degeneration Progresses Averaged Across all Trials"
  SubTitle <- paste0(figTitle, " - Target Centroid Position is ", targetCentroid)
  
  # Plot the average deviation with shaded error margins, using degeneration percentages as x-axis
  p <- ggplot(plot_data, aes(x = DegenerationPercentage, y = Avg)) +
    geom_ribbon(aes(ymin = LowerBound, ymax = UpperBound), alpha = 0.5, fill = "#B7E4D9") +
    geom_line(aes(y = Avg), color = '#B3CDE0', size = 1.5) +
    labs(title = Title,
         subtitle = SubTitle,
         x = "Degeneration Percentage (%)",
         y = "Absolute Deviation") +
    theme_classic() +
    theme(
      text = element_text(size = 14, family = "Times New Roman"),  # Use Times New Roman font throughout
      plot.title = element_text(face = "bold", size = 16),  # Bold title
      plot.subtitle = element_text(size = 12),
      axis.title = element_text(face = "bold"),  # Bold axis titles
      panel.grid.major = element_line(color = "lightgray", size = 0.5)
    ) +
    scale_x_continuous(breaks = seq(0, max(trimmed_deg_per), by = deg_per * 10), expand = c(0, 0)) +  # Adjust x-axis for degeneration percentage
    scale_y_continuous(expand = c(0, 0))  # Start y-axis at 0
  
  # Save the plot
  filename <- gsub('[^[:alnum:]]', '', paste(figTitle, targetCentroid))
  ggsave(filename = paste0(locationDirPath, experimentName, "/", filename, ".png"), plot = p, width = 10, height = 6)
}


# Define the function to generate a combined plot for each experiment
generate_combined_plot <- function(deviations_list, targetCentroids, figTitle, experimentName) {
  
  # List to store individual plots
  plot_list <- list()
  
  # Loop over each deviation and create the corresponding plot
  for (i in 1:length(deviations_list)) {
    deviations <- deviations_list[[i]]
    targetCentroid <- targetCentroids[i]
    
    # Calculate average and standard error
    avgLine <- apply(deviations, 2, mean, na.rm = TRUE)
    semLine <- apply(deviations, 2, std.error, na.rm = TRUE)
    iterations <- seq_along(avgLine)
    
    # Find the index of the maximum value in avgLine
    max_index <- which.max(avgLine)
    
    # Trim data to only include values up to the maximum y-value
    trimmed_iterations <- iterations[1:max_index]
    trimmed_avgLine <- avgLine[1:max_index]
    trimmed_semLine <- semLine[1:max_index]
    
    # Create a data frame for plotting the trimmed data
    plot_data <- data.frame(
      Iterations = trimmed_iterations,
      Avg = trimmed_avgLine,
      LowerBound = trimmed_avgLine - trimmed_semLine,
      UpperBound = trimmed_avgLine + trimmed_semLine
    )
    
    # Plot the average deviation for the current centroid
    p <- ggplot(plot_data, aes(x = Iterations, y = Avg)) +
      geom_ribbon(aes(ymin = LowerBound, ymax = UpperBound), alpha = 0.5, fill = "#B7E4D9") +
      geom_line(aes(y = Avg), color = '#B3CDE0', size = 1.5) +
      labs(subtitle = paste("Target Centroid:", targetCentroid),
           x = "Simulation Iterations",
           y = "Absolute Deviation") +
      theme_classic() +
      theme(
        text = element_text(size = 14, family = "Times New Roman"),  # Use Times New Roman font throughout
        plot.title = element_text(face = "bold", size = 16),  # Bold title
        plot.subtitle = element_text(size = 12),
        axis.title = element_text(face = "bold"),  # Bold axis titles
        panel.grid.major = element_line(color = "lightgray", size = 0.5),
      ) +
      scale_x_continuous(breaks = seq(0, 500, by = 10), expand = c(0, 0)) +  # Start x-axis at 0
      scale_y_continuous(expand = c(0, 0))  # Start y-axis at 0
    
    # Add the plot to the list
    plot_list[[i]] <- p
  }
  
  # Combine all 7 plots into a single figure with a custom layout
  combined_plot <- arrangeGrob(
    grid.arrange(grobs = plot_list, ncol = 2),  # Arrange plots in 2 columns
    layout_matrix = rbind(c(1, 1),
                          c(1, 1),
                          c(1, 1))  # Custom layout for the legend
  )
  
  # Add title using grid.text
  final_plot <- grid.arrange(
    combined_plot, 
    top = textGrob(figTitle, gp = gpar(fontsize = 20, fontface = "bold", fontfamily = "Times New Roman")),
    padding = unit(1, "line")  # Add some padding
  )
  
  # Save the combined plot
  filename <- paste0(experimentName, "-combined-plot.png")
  ggsave(filename = file.path(plotFilePath, filename), plot = final_plot, width = 12, height = 8)
}

# Loop through each experiment
for (experiment in 1:nrow(experiments)) {
  
  # Define the title based on experiment
  experimentTitle <- switch(experiments$experiment_name[experiment],
                            'deactivate pre-synaptic neurons' = paste("Deactivating One Unique", experiments$variable[experiment]),
                            'deactivate post-synaptic neurons' = paste("Deactivating One Unique", experiments$variable[experiment]),
                            'reduce 0.005 weights' = paste("Randomly Reducing One", experiments$variable[experiment], "by 95%"),
                            'randomize weights' = paste("Randomly Randomizing One", experiments$variable[experiment]),
                            'deactivate weights' = paste("Randomly Deactivating One Unique", experiments$variable[experiment])
  )

  # Initialize a list to store deviations for all positions
  deviations_list <- list()
  
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
    
    # Generate and save the plot
    plot_centroid_avg_deviation(deviations, plotFilePath, experimentTitle, subTitle, 
                                targetCentroid, experiments$experiment_name[experiment],
                                experiments$deg_per[experiment])
    
    # Append deviations to the list
    deviations_list[[positionIndex]] <- deviations
  }
  
  # Generate and save the combined plot for the current experiment
  generate_combined_plot(deviations_list, targetCentroids, experimentTitle, experiments$experiment_name[experiment])
  
}