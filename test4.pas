program TestCase5;

variabel
  x: integer;

mulai
  untuk x := 1 ke 10
    writeln(x); { ERROR: Missing keyword `lakukan` }
selesai.