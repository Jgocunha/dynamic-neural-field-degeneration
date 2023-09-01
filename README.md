# dynamic-neural-field-degeneration

Repository for conducting an experiment on dynamic neural field degeneration. 
This project explores the phenomenon of degeneration in dynamic neural fields and its impact on cognitive processes.
It includes the source files for the dynamic-neural-field-composer and coppeliasim-cpp libraries, along with custom code for running the experiment.

## Goals
Specifically, our objectives encompassed:
- Understanding the threshold at which the DNF system exhibits complete misbehavior.
- Identifying degenerative conditions that lead to a faster decay of the system.
- Exploring the potential for degeneration to be reverted, investigating whether DNF models possess artificial "neuroplasticity" – the capability to reorganize their neural architecture and restore the original correct behavior.

## Experiment Overview
To address these research questions, we designed two distinct experimental scenarios:

1. **Continuous Degeneration Experiment**:
   We implemented a basic DNF architecture and subjected it to continuous degeneration, emulating neuron death, synaptic connection decline, and disruption. By exploring various degenerative conditions and comparing their impacts, we aimed to determine the threshold for complete misbehavior and identify conditions leading to faster decay.

2. **Simulated Robotics Experiment**:
   In a (simulated) robotics experiment, we introduced degenerative conditions to a robot's cognitive architecture, leading to functional deterioration. We delved into the potential for degeneration reversion and artificial "neuroplasticity". We examined if reinforcement learning techniques could facilitate neural connection formation or reorganization within the degraded architecture, with the goal of restoring the system's original correct behavior.

## Methodology

![neural-architecture](https://github.com/Jgocunha/dynamic-neural-field-degeneration/assets/63354863/ad8de05f-8bde-42df-a5f2-0008500ea0c4)

*Two coupled dynamic neural fields - the perceptual and the decision fields.
The perceptual field receives inputs from the vision system, while the decision field obtains stimulus through inter-field weights. 
Each neural position within the perceptual field establishes connections with every neural position in the decision field.
The decision field encodes the target placement angle for the robotic arm.*

![simulation-setup](https://github.com/Jgocunha/dynamic-neural-field-degeneration/assets/63354863/a1c5271f-b352-43b5-b05c-6374d972fb41)

*The left side of the figure offers insight into the dynamic responses within the activation of the neural fields upon encountering input from a yellow cuboid. 
The perceptual field captures the cuboid's distinct hue. This stimulus cascades into the decision field, fostering the emergence of a corresponding bump. 
The centroid of this resultant bump marks the target placement angle destined for the robotic arm. 
Utilizing this value, the robot proceeds to pick and place the cuboid into container number 3, corresponding to the target placement angle.*

Current experiment methodology:
```
Set successfullPickAndPlace to true
Repeat
    For i from 0 to param.numberOfTrials
        Repeat
            Set successfullPickAndPlace to pickAndPlace()
            If not successfullPickAndPlace
                Run relearningProcedure()
                Increment stats.numOfRelearningCycles by 1
            End If
        Until (not successfullPickAndPlace and stats.numOfRelearningCycles < 100)
        Run saveLearningCyclesPerTrial()
        Run cleanUpTrial()
    End For
    Increment param.currentPercentageOfDegeneration by param incrementOfDegenerationPercentage
    Run degenerationProcedure()
Until param.currentPercentageOfDegeneration < param targetPercentageOfDegeneration
```

## Results

TO DO!
