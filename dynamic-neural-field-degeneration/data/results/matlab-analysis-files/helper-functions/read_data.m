function [data] = read_data(centroidFilePath)
        % Open the file for reading

        fid = fopen(centroidFilePath, 'r');
        % Initialize an empty cell array to store the lines
        lines = cell(0);

        % Read the file line by line
        tline = fgetl(fid);
        while ischar(tline)
            % Convert the line to numeric values
            lineData = str2double(strsplit(tline));
            
            % Remove NaN and values equal to 0
            lineData = lineData(~isnan(lineData) & lineData ~= 0);
            
            % Add the line to the cell array if it's not empty
            if ~isempty(lineData)
                lines = [lines; {lineData}];
            end
            
            % Read the next line
            tline = fgetl(fid);
        end
        
        % Close the file
        fclose(fid);
        
        data = lines;
end
