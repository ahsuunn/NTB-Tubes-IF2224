program TestCase5;

variabel
  angka: integer;
  teks: char;

mulai
  teks := 'A';
  angka := 10 + teks; { ERROR: Type mismatch }
selesai.