program TestExpressions;

variabel
  a, b, c: integer;
variabel
  hasil: integer;
variabel
  benar: boolean;

mulai
  a := 5;
  b := 10;
  c := a + b;
  c := a - b;
  c := a * b;
  c := a bagi b;
  c := a mod b;
  benar := a = b;
  benar := a <> b;
  benar := a < b;
  benar := a <= b;
  benar := a > b;
  benar := a >= b;
  benar := (a < b) dan (b < 20);
  benar := (a > b) atau (b > 5);
  benar := tidak (a = b);
  hasil := (a + b) * (c - 5) bagi 2;
  writeln('Hasil akhir = ', hasil)
selesai.
