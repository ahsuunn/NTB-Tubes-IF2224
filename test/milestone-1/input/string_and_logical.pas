program LogicTest;
var
  flag: boolean;
begin
  flag := (5 < 10) and not (3 > 1);
  writeln('Check: ', flag);
end.
