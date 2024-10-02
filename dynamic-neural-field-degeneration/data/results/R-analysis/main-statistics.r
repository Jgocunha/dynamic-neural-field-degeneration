# Load necessary libraries
library(tidyverse)

# Define the directory containing the files
data_dir <- "../"

# Get a list of all files that match the naming pattern
file_list <- list.files(path = data_dir, pattern = "*.txt", full.names = TRUE)

# Initialize an empty list to store data frames
data_list <- list()

# Function to extract the position and condition from the filename
extract_info <- function(filename) {
  # Remove the directory path and extension
  file_name <- basename(filename)
  # Split the name based on space
  parts <- strsplit(file_name, " - ")[[1]]
  position_condition <- strsplit(parts[1], " ")[[1]]
  list(position = position_condition[1], 
       condition = paste(position_condition[-1], collapse = " "))
}

# Loop through each file, read it, and store with relevant metadata
for (file in file_list) {
  # Extract the position and condition
  file_info <- extract_info(file)
  
  # Read the data without expecting a fixed number of columns
  # Use `fill = TRUE` to fill missing values with NA if row lengths vary
  data <- read.table(file, header = FALSE, fill = TRUE)
  
  # Add position and condition as columns
  data$position <- file_info$position
  data$condition <- file_info$condition
  
  # Store the data in the list
  data_list[[file]] <- data
}

# Combine all the data into one data frame
final_data <- bind_rows(data_list)

# Summarize the data: calculate the mean for each condition and position
summary_data <- final_data %>%
  group_by(position, condition) %>%
  summarize(across(starts_with("V"), mean, na.rm = TRUE))

print(summary_data)

# Define the target positions (must be numeric for calculations)
target_positions <- c(2.0, 6.0, 10.0, 14.0, 18.0, 22.0, 26.0)

# Function to find the first iteration where deviation exceeds +/- 2 degrees
find_deviation <- function(bump_values, target_position) {
  # Calculate the deviation from the target position
  deviation <- abs(bump_values - target_position)
  
  # Find the first index where deviation exceeds 2 degrees
  first_deviation <- which(deviation > 1.8)
  
  # If no deviation is found, return NA; otherwise, return the iteration (column index)
  if (length(first_deviation) == 0) {
    return(NA)
  } else {
    return(first_deviation[1])
  }
}

# Reshape the data into long format and then process row-wise
final_data_long <- final_data %>%
  pivot_longer(cols = starts_with("V"), names_to = "iteration", values_to = "bump_value") %>%
  group_by(position, condition) %>%
  mutate(target_position = as.numeric(position)) %>%
  arrange(position, condition, iteration)

# Apply the find_deviation function row-wise
final_data <- final_data_long %>%
  group_by(position, condition) %>%
  summarize(deviation_iter = find_deviation(bump_value, unique(target_position)),
            .groups = "drop")

# Group by condition and position, then compute the average iterations for deviation
average_deviation <- final_data %>%
  group_by(condition, position) %>%
  summarize(average_deviation_iter = mean(deviation_iter, na.rm = TRUE))

# Display the results
print(average_deviation)
