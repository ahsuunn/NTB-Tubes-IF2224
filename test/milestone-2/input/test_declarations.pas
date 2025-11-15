program TestDeclarations;

konstanta 
  MAX = 100;
  MIN = 0;
  PI = 3.14;

tipe 
  Range = 1..10;
  Matrix = larik[1..5] dari integer;

variabel
  x, y, z: integer;
  nilai: real;
  huruf: char;
  valid: boolean;
  angka: Range;
  data: Matrix;

mulai
  x := 10;
  y := 20;
  z := x + y;
  writeln('Hasil = ', z)
selesai.
