function [dataMatrix] = read_data(filePath)
% Open the file for reading
fileID = fopen(filePath, 'r');

% Check if the file was opened successfully
if fileID == -1
    error('Unable to open the file.');
end

% Initialize a cell array to store data lines
dataCell = {};

% Read data from the file
while ~feof(fileID)
    line = fgetl(fileID);
    if ischar(line)
        % Split the line into individual values based on spaces
        line_values = strsplit(line);
        
        % Convert the cell array of strings to a numeric array
        line_data = str2double(line_values);
        
        % Append the numeric array to the cell array
        dataCell{end+1} = line_data;
    end
end

% Close the file
fclose(fileID);

% Determine the maximum number of columns in the data
max_columns = max(cellfun(@numel, dataCell));

% Pad shorter lines with zeros
dataCell = cellfun(@(x) [x, zeros(1, max_columns - numel(x))], dataCell, 'UniformOutput', false);

% Convert the cell array of padded arrays to a numeric matrix
dataMatrix = vertcat(dataCell{:});
end

