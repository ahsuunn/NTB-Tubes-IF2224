program TestComprehensive;

konstanta
  MAX_SIZE = 100;
  VERSION = 2;

tipe
  IndexRange = 1..10;
  DataArray = larik[1..10] dari integer;

variabel
  arr: DataArray;
  i, sum, avg: integer;
  found: boolean;

fungsi FindMax(data: DataArray; size: integer): integer;
variabel
  max, idx: integer;
mulai
  max := data;
  untuk idx := 2 ke size lakukan
    jika data > max maka
      max := data;
  FindMax := max
selesai;

prosedur PrintArray(data: DataArray; count: integer);
variabel
  j: integer;
mulai
  untuk j := 1 ke count lakukan
    write(data, ' ')
selesai;

mulai
  { Initialize array }
  sum := 0;
  untuk i := 1 ke 10 lakukan
    mulai
      arr := i * 5;
      sum := sum + arr
    selesai;
  
  { Calculate average }
  avg := sum bagi 10;
  
  { Find maximum }
  i := FindMax(arr, 10);
  
  { Print results }
  writeln('Array values:');
  PrintArray(arr, 10);
  writeln();
  writeln('Sum = ', sum);
  writeln('Average = ', avg);
  writeln('Maximum = ', i);
  
  { Search for value }
  found := salah;
  i := 1;
  selama (i <= 10) dan (tidak found) lakukan
    mulai
      jika arr = 25 maka
        found := benar
      selain-itu
        i := i + 1
    selesai;
  
  jika found maka
    writeln('Value 25 found at index ', i)
  selain-itu
    writeln('Value 25 not found')
selesai.
