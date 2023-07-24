function [avgNumIt] = getAvgIt(lines)
lineCount = numel(lines);
totalSize = 0;
for i = 1:lineCount
    totalSize = totalSize + size(lines{i});
end
aux = totalSize / lineCount;
avgNumIt = aux(2);
end

