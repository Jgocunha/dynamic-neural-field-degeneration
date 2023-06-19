#pragma once

#include <exception>
#include <string>

enum class ErrorCode : int
{
	OK,
	APP_CONSTRUCTION, APP_INIT, APP_STEP, APP_CLOSE,
	SIM_ELEM_NOT_FOUND, SIM_RUNTIME_LESS_THAN_ZERO, SIM_ELEM_INDEX, SIM_ELEM_ALREADY_EXISTS,
	ELEM_INPUT_IS_NULL, ELEM_INPUT_ALREADY_EXISTS, ELEM_INPUT_NOT_FOUND, ELEM_INPUT_SIZE_MISMATCH, 
	ELEM_SIZE_NOT_ALLOWED, ELEM_RENAME_NOT_ALLOWED, ELEM_COMP_NOT_FOUND,
	GAUSS_STIMULUS_POSITION_OUT_OF_RANGE, GAUSS_STIMULUS_SUM_MISMATCH,
	VIS_INVALID_SIM, VIS_DATA_NOT_FOUND, VIS_INVALID_PLOTTING_INDEX,
};

class Exception : public std::exception
{
private:
	ErrorCode errorCode;
	std::string errorMessage;
	std::string errorElement;
	std::string errorComponent;
	int errorIndex;
public:
	Exception(const std::string& message);
	Exception(const ErrorCode errorCode);
	Exception(const ErrorCode errorCode, const std::string& errorElement);
	Exception(const ErrorCode errorCode, int errorIndex);
	Exception(const ErrorCode errorCode, const std::string& errorElement, const std::string& errorComponent);

	const char* what() const noexcept override;

	ErrorCode getErrorCode() const;
	std::string getErrorMessage();
};