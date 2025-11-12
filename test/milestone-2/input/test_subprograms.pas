program TestProceduresAndFunctions;

variabel
  x, y, hasil: integer;
variabel
  nama: char;

prosedur PrintHello(msg: char);
variabel
  temp: integer;
mulai
  temp := 0;
  writeln(msg)
selesai;

prosedur Swap(a, b: integer);
variabel
  temp: integer;
mulai
  temp := a;
  a := b;
  b := temp
selesai;

fungsi Tambah(a, b: integer): integer;
variabel
  result: integer;
mulai
  result := a + b;
  Tambah := result
selesai;

fungsi Kali(x: integer; y: integer): integer;
mulai
  Kali := x * y
selesai;

mulai
  x := 10;
  y := 20;
  
  { Call procedures }
  PrintHello('A');
  Swap(x, y);
  
  { Call functions }
  hasil := Tambah(x, y);
  hasil := Kali(5, 6);
  hasil := Tambah(Kali(2, 3), 10);
  
  writeln('Hasil = ', hasil)
selesai.
