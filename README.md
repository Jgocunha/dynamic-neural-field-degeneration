# dynamic-neural-field-degeneration

![Alt text](https://res.cloudinary.com/doyaoy7aj/image/upload/v1730798144/dnf-deg-logo.png)

---

[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.31%2B-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org)
[![R](https://img.shields.io/badge/R-analysis-276DC3?style=flat-square&logo=r&logoColor=white)](https://www.r-project.org)
[![Windows](https://img.shields.io/badge/Windows-0078D6?style=flat-square&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI%2BPHBhdGggZmlsbD0iI2ZmZiIgZD0iTTAgMy40NDlMOS43NSAyLjF2OS40NTFIMG0xMC45NDktOS42MDJMMjQgMHYxMS40SDEwLjk0OU0wIDEyLjZoOS43NXY5LjQ1MUwwIDIwLjY5OU0xMC45NDkgMTIuNkgyNFYyNGwtMTIuOS0xLjgwMSIvPjwvc3ZnPg%3D%3D&logoColor=white)](https://github.com/Jgocunha/dynamic-neural-field-degeneration)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=flat-square)](LICENSE)

## Overview

**dynamic-neural-field-degeneration** is a framework designed to simulate and study the effects of 
artificial neural degeneration on **Dynamic Neural Field (DNF) models**. 
This work explores the resilience and adaptability of DNFs, specifically in the context of 
cognitive behaviour in robotic systems. The project centres on a **robotic sorting task** 
where the impact of degeneration on task performance is tested and visualised. 

Key objectives:
- **Investigate resilience** of DNFs under neuron death and synaptic disruption.
- **Explore adaptability** through relearning mechanisms to recover functionality post-degeneration.
- **Assess biological analogues** by modelling degenerative effects inspired by conditions such as 
Huntington’s, Alzheimer’s, and Parkinson’s diseases.

## Features 

- **Dynamic neural field simulation and visualization**: This project runs [dynamic-neural-field-composer](https://github.com/Jgocunha/dynamic-neural-field-composer) 
for the creation, composition and real-time visualisation of neural field dynamics. 
- **Degeneration modelling**: Induce artificial neurodegenerative conditions (e.g., neuron death, synaptic 
connection degradation) and observe their impact.
- **Relearning and recovery**: Implement mechanisms to reorganise connections and recover functionality in 
degraded architectures.
- **Robotic sorting task integration**: The experiment is grounded in a simulated, using CoppeliaSim, sorting task with a UR10 
robotic manipulator.

## Installation

To run this project, ensure you have the following dependencies installed:

1. **[dynamic-neural-field-composer](https://github.com/Jgocunha/dynamic-neural-field-composer)**: Clone the repository and follow the provided building and installation instructions.
2. **[coppeliasim-cpp](https://github.com/Jgocunha/coppeliasim-cpp)**: Again, clone the repository and follow its building and installation instructions.

Once dependencies are installed, you can build this project using the `build.bat` file provided in the root directory.

**Executable Files:**
- To simulate degeneration, run `inducing-degeneration.exe`.
- To simulate recovery and relearning, run `recovering-from-degeneration.exe`.

## Usage 

To replicate specific experiments, switch to the corresponding Git branch, as follows:

- **Inducing Degeneration:** `git checkout inducing-degeneration`.
- **Relearning from Degeneration:** `git checkout recovering-from-degeneration`.

### Modifying Experiment Parameters

To adjust the simulation parameters for either experiment, edit the `experiment_parameters.json` file to configure factors such as neuron behavior, synaptic degradation rates, or other simulation settings.

### Viewing the Robotic Simulation

For the relearning experiment, which is coupled with a robotic simulation, you can view the sorting task:

1. `git checkout coppeliasim-visualisation`.
1. Open the `scenario.ttt` scene file in **CoppeliaSim**.
2. Press **Play** in CoppeliaSim before or after running the `recovering-from-degeneration.exe` executable to observe the robotic system performing the task.


## Video explanation

[![Watch the video](https://img.youtube.com/vi/SsyoWnwk_bQ/maxresdefault.jpg)](https://youtu.be/SsyoWnwk_bQ)

## Documentation

For a full exploration of the repository, refer to the Wiki.

## Citation

If you use this work in your research, please cite:

> J. G. Cunha, R. H. Cuijpers, W. Erlhagen, E. Bicho, "Robustness and Adaptability in a Dynamic Neural Field Architecture Subject to Degeneration," in: *ROBOT 2025 – 8th Iberian Conference*, status: In press.
