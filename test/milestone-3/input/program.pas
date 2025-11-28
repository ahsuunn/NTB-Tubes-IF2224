program TestTypesComplete;

tipe
    IntRange = 1..100;
    MyArray = larik[1..10] dari integer;
    Matrix = larik[1..5] dari real;
    
variabel
    x, y: IntRange;
    arr: MyArray;
    mat: Matrix;
    total: integer;

mulai
    x := 10;
    y := 20;
    total := 0;
    
    untuk x := 1 ke 10 lakukan mulai
        total := total + x
    selesai;
    
    writeln('Total: ');
    writeln(total)
selesai.