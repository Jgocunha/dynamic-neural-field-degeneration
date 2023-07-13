#pragma once

#include "user_interface/user_interface_window.h"

#include "./wizards/field_coupling_wizard.h"

class CouplingWindow : public UserInterfaceWindow
{
private:
	std::shared_ptr<Simulation> simulation;
	std::shared_ptr<Visualization> visualization;

	FieldCouplingWizard fieldCouplingWizard;
	ImVec2 mouseCoordinates;

public:
	CouplingWindow(const std::shared_ptr<Simulation>& simulation);
	void render() override;
	~CouplingWindow() = default;
private:
	void renderCouplingSelector();
	void renderPlots();
	void renderOperations();
	void checkForMousePress();
	void renderAddStimulusButtons();
	void renderRemoveStimulusButton();
	void renderFinishTrainingButton();
	void renderStimulusFinishedButton();
};