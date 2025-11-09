program TestOps;
var
  a, b: integer;
  c: char;
begin
  a := 3;
  b := 4;
  if a <= b then
    c := 'x';
  for a := 1 to 10 do
    b := b + a;
end.
