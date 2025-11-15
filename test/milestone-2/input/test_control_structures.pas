program TestControlStructures;

variabel
  i, n, hasil: integer;
  kondisi: boolean;

mulai
  n := 10;
  hasil := 0;
  jika n > 5 maka
    hasil := n * 2;
  jika n < 5 maka
    hasil := n + 10
  selain-itu
    hasil := n - 5;
  jika n > 0 maka
    jika n < 100 maka
      hasil := n
    selain-itu
      hasil := 100
  selain-itu
    hasil := 0;
  i := 1;
  selama i <= 5 lakukan
    mulai
      hasil := hasil + i;
      i := i + 1
    selesai;
  untuk i := 1 ke 10 lakukan
    hasil := hasil + i;
  untuk i := 10 turun-ke 1 lakukan
    hasil := hasil - 1;
  untuk i := 1 ke 3 lakukan
    mulai
      n := 1;
      selama n <= 2 lakukan
        mulai
          hasil := hasil + 1;
          n := n + 1
        selesai
    selesai;
  writeln('Hasil akhir = ', hasil)
selesai.
