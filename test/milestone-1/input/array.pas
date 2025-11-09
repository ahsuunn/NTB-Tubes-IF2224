program ArrayTest;
var
  nums: array [1..5] of integer;
  i: integer;
begin
  for i := 1 to 5 do
    writeln('Element ', i, ': ', nums[i]);
end.
