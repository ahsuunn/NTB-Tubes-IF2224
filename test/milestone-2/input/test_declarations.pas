program TestDeclarations;

konstanta MAX = 100;
konstanta MIN = 0;
konstanta PI = 3.14;

// tipe Range = 1..10;
// tipe Matrix = larik[1..5] dari integer;

variabel
  x, y, z: integer;
variabel
  nilai: real;
variabel
  huruf: char;
variabel
  valid: boolean;
// variabel
//   angka: Range;
// variabel
//   data: Matrix;

mulai
  x := 10;
  y := 20;
  z := x + y;
  writeln('Hasil = ', z)
selesai.
