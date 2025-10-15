program Hello;
var
  a, b : integer;
begin
  a := 10;
  b := a + 20;
  (* this is a comment *)
  { another comment }
  if a < b then
    b := b - 1;
end.
