# Load required libraries
library(ggplot2)
library(extrafont)
library(dplyr)
library(tidyr)
library(svglite)
library(plotrix)
library(patchwork)
library(gghalves)  # optional for better half-violin + jitter combo

#font_import()
loadfonts(device="all")

# List available Windows fonts
fonts <- windowsFonts()
#print(names(fonts))

# Get the script directory
if (interactive()) {
  script_directory <- rstudioapi::getActiveDocumentContext()$path
  script_directory <- dirname(script_directory)
} else {
  script_directory <- getwd()
}
setwd(script_directory)

# Load the data
current_directory <- getwd()

# Define the function to read data from the file
read_data <- function(filePath) {
  dataMatrix <- as.matrix(read.table(filePath, header = FALSE, fill = TRUE))
  maxColumnSize <- ncol(dataMatrix)
  return(list(dataMatrix = dataMatrix, maxColumnSize = maxColumnSize))
}

# Set experiment parameters
resultPath <- '../results/'
degeneracyType <- 'deactivate post-synaptic neurons'
relearningType <- 'Only-degenerated-cases'
epochs <- 1
maximumLearningCycles <- 50
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
initialPer <- 80
finalPer <- 100
incPer <- 0.36
finalPer <- initialPer + (incPer * maxColumns) - 1 * incPer
degenerationPercentages <- round(seq(initialPer, finalPer, by = incPer), 2)

# Initialize result variables
numCorrectBehaviour <- rep(0, ncol(dataMatrix))
numFailedBehaviour <- rep(0, ncol(dataMatrix))
numRecoveredBehaviour <- rep(0, ncol(dataMatrix))
numDeadFields <- rep(0, ncol(dataMatrix))
avgRelearningCycles <- rep(0, ncol(dataMatrix))
stdErrRelearningCycles <- rep(0, ncol(dataMatrix))
numValidElements <- rep(0, ncol(dataMatrix))

perCorrectBehaviour <- rep(0, ncol(dataMatrix))
perFailedBehaviour <- rep(0, ncol(dataMatrix))
perRecoveredBehaviour <- rep(0, ncol(dataMatrix))
perDeadFields <- rep(0, ncol(dataMatrix))

# Iterate through each column
for (col in 1:ncol(dataMatrix)) {
  columnData <- dataMatrix[, col]
  numDeadFields[col] <- sum(is.na(columnData))
  validData <- columnData[!is.na(columnData)]
  numFailedBehaviour[col] <- sum(validData > 0) + numDeadFields[col]
  numRecoveredBehaviour[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  numCorrectBehaviour[col] <- nrow(dataMatrix) - numFailedBehaviour[col]
  avgRelearningCycles[col] <- mean(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  stdErrRelearningCycles[col] <- std.error(validData[validData > 0 & validData < maximumLearningCycles], na.rm = TRUE)
  numValidElements[col] <- sum(validData > 0 & validData < maximumLearningCycles)
  perCorrectBehaviour[col] <- numCorrectBehaviour[col] / nrow(dataMatrix) * 100
  perRecoveredBehaviour[col] <- numRecoveredBehaviour[col] / numFailedBehaviour[col] * 100
  perFailedBehaviour[col] <- numFailedBehaviour[col] / nrow(dataMatrix) * 100
  perDeadFields[col] <- numDeadFields[col] / nrow(dataMatrix) * 100
}

# Create results data frame
results <- data.frame(
  Degeneracy = degenerationPercentages,
  CorrectBehaviour = perCorrectBehaviour,
  FailedBehaviour = perFailedBehaviour,
  RecoveredBehaviour = perRecoveredBehaviour,
  AvgRelearningCycles = avgRelearningCycles,
  StdErrRelearningCycles = stdErrRelearningCycles,
  NumValidElements = numValidElements,
  DeadFields = perDeadFields
)

# Behavior y-axis scaling
bp_max <- 100
bp_min <- 0

# Filtered results for cycles plot
results_filtered_non_zero <- results %>%
  filter(AvgRelearningCycles > 0)
results_filtered_zero <- results %>%
  filter(AvgRelearningCycles == 0)

# Font settings
font <- "Times New Roman"
font_size <- 24

# Colors
behavior_colors <- c(
  "FailedBehaviour" = "#D73027",    # Red (medium dark)
  "RecoveredBehaviour" = "#1A9850"  # Green (medium)
)

# Shared x-axis breaks
x_breaks <- pretty(results$Degeneracy, n = 8)
x_limits <- range(results$Degeneracy)

# Pivot full results (not filtered)
results_long <- results %>%
  select(Degeneracy, FailedBehaviour, RecoveredBehaviour) %>%
  pivot_longer(cols = -Degeneracy, names_to = "BehaviourType", values_to = "Percentage")

# Top plot
p_behavior <- ggplot(results_long, aes(x = Degeneracy, y = Percentage, color = BehaviourType)) +
  geom_line(size = 1.5) +
  geom_point(size = 3) +
  #geom_smooth(method = "loess", se = TRUE, linetype = "dashed", size = 0.8, alpha = 0.5) +
  scale_color_manual(
    values = behavior_colors,
    labels = c("Failed behaviour", "Recovered behaviour")
  ) +
  scale_y_continuous(name = "Behavior (%)", limits = c(0, 100), breaks = seq(0, 100, by = 20)) +
  scale_x_continuous(breaks = x_breaks, limits = x_limits, labels = NULL) +
  theme_minimal(base_size = font_size) +
  theme(
    text = element_text(family = font, size = font_size),
    axis.title.x = element_blank(),
    axis.text.x = element_blank(),
    axis.ticks.x = element_blank(),
    axis.title.y = element_text(face = "bold"),
    legend.position = c(0.175, 0.85),  # x, y inside plot (adjust as needed)
    legend.background = element_rect(fill = alpha('white', 0.6), color = NA),
    #legend.key = element_rect(fill = NA),
    legend.title = element_blank()
  )

# Prepare data and remove statistical outliers
raw_data <- data.frame(
  Degeneracy = rep(degenerationPercentages, each = nrow(dataMatrix)),
  RelearningCycles = as.vector(dataMatrix)
) %>%
  filter(!is.na(RelearningCycles) & RelearningCycles > 0 & RelearningCycles < maximumLearningCycles)

# Compute IQR and filter outliers
Q1 <- quantile(raw_data$RelearningCycles, 0.25)
Q3 <- quantile(raw_data$RelearningCycles, 0.75)
IQR_val <- Q3 - Q1
lower_bound <- Q1 - 1.5 * IQR_val
upper_bound <- Q3 + 1.5 * IQR_val

jitter_data <- raw_data %>%
  filter(RelearningCycles >= lower_bound & RelearningCycles <= upper_bound)

# Prepare data for legend
jitter_data$Type <- "Individual cycles"
results$Type <- "Mean cycles"

# Combine for plotting with legend
plot_data_points <- jitter_data
plot_data_points$Type <- "Individual cycles"
plot_data_mean <- results
plot_data_mean$Type <- "Mean cycles"

p_cycles <- ggplot() +
  geom_jitter(data = plot_data_points, aes(x = Degeneracy, y = RelearningCycles, color = Type),
              width = 0.3, height = 0.3, size = 1.5, alpha = 0.4) +
  geom_line(data = plot_data_mean, aes(x = Degeneracy, y = AvgRelearningCycles, color = Type), size = 1.5) +
  geom_point(data = plot_data_mean, aes(x = Degeneracy, y = AvgRelearningCycles, color = Type), size = 2) +
  scale_color_manual(
    name = NULL,  # removes legend title
    values = c("Individual cycles" = "black", "Mean cycles" = "#4575B4")
  ) +
  scale_y_continuous(
    name = "Relearning Cycles",
    limits = c(1, 1.3),
    breaks = seq(0, 12, by = 0.1)
  ) +
  scale_x_continuous(name = "Degeneration (%)",
                     breaks = x_breaks,
                     limits = x_limits) +
  theme_minimal(base_size = font_size) +
  theme(
    text = element_text(family = font, size = font_size),
    axis.title.x = element_text(face = "bold"),
    axis.title.y = element_text(face = "bold"),
    legend.position = c(0.15, 0.85),  # inside bottom-right corner (adjust)
    legend.background = element_rect(fill = alpha('white', 0.6), color = NA),
    #panel.grid.minor = element_blank()
  )

# Combine plots
combined_plot <- p_behavior / p_cycles + 
  plot_layout(heights = c(2, 1))

print(combined_plot)

# Save plot
new_width <- 10
new_height <- (new_width / 3.5) * 3
plot_filename <- paste0("./plots/line_plots", degeneracyType, ' ', relearningType, 
                        ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                        ' Update-all-weights-', updateAllWeights, '.svg')

#ggsave(
#  filename = plot_filename,
#  plot = combined_plot,
#  device = "svg",
#  width = new_width,
#  height = new_height,
#  units = "in"
#)

print(plot_filename)

# Save results
results_filename <- paste0("./analysis/", degeneracyType, ' ', relearningType, 
                           ' Epochs-', epochs, ' ', 'MaxCycles-', maximumLearningCycles, 
                           ' Update-all-weights-', updateAllWeights, '.txt')

write.table(
  results,
  file = results_filename,
  sep = "\t",
  row.names = FALSE,
  col.names = TRUE,
  quote = TRUE
)

print(results_filename)
