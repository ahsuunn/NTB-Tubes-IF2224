# Milestone 1 Pascal Lexical Analyzer (paslex)

Milestone ini berisi sebuah program penganalisis leksikal (lexer) untuk sebagian dari bahasa pemrograman Pascal-S, yang dibangun sebagai bagian dari Tugas Besar mata kuliah Teori Bahasa Formal dan Otomata (IF2224).

## Identitas Kelompok

-   **Kode Kelompok**: NTB
-   **Nama Kelompok**: Ahsan Et Al
-   **Anggota**:
    -   Raka Daffa Iftikhaar (13523018)
    -   Aliya Husna Fayyaza (13523062)
    -   Ahsan Malik Al Farisi (13523074)
    -   Bevinda Vivian (13523120)

## Deskripsi Program
Program ini adalah penganalisis leksikal yang menerima sebuah file sumber Pascal-S (`.pas`) dan menghasilkan serangkaian token berdasarkan aturan leksikal yang didefinisikan dalam sebuah *Deterministic Finite Automaton* (DFA). Program ini mampu membedakan berbagai jenis token seperti *keywords*, *identifiers*, *numbers*, *operators*, *string literals*, dan simbol lainnya. DFA yang digunakan untuk tokenisasi dapat dimuat dari file berformat `.json`.

## Requirements
-   Compiler C++ yang mendukung standar C++17 (contoh: `g++`).
-   `make` untuk membangun proyek.
-   `WSL`atau lingkungan UNIX untuk membantu menjalankan perintah make di Windows (opsional).

## Cara Instalasi dan Penggunaan Program

### Instalasi

1.  Clone repository ini ke dalam direktori lokal Anda:
    ```sh
    git clone https://github.com/ahsuunn/NTB-Tubes-IF2224
    ```
2.  Pastikan Anda memiliki `g++` dan `make` terinstal di sistem Anda.
3.  Buka terminal di direktori root proyek NTB-Tubes-IF2224.
4.  Jalankan perintah berikut untuk mengkompilasi program (Sudah dijalankan, tidak perlu dijalankan ulang, jika paslex tidak ada pada root silakan jalankan ulang):
    ```sh
    make build
    ```
    Perintah ini akan menghasilkan sebuah file eksekusi bernama `paslex` di direktori root.

### Penggunaan

Program dapat dijalankan melalui terminal dengan format berikut:

```sh
./paslex <source.pas> [--dfa path/to/dfa.json|dfa.txt]
```

-   `<source.pas>`: Path menuju file kode sumber Pascal yang akan dianalisis.
-   `--dfa` (opsional): Flag untuk menentukan path file DFA. Jika tidak disertakan, program akan menggunakan `dfa/dfa.json` secara default.

**Untuk Memudahkan Penggunaan**

    wsl #masuk ke dalam wsl jika di windows
    make run

Perintah ini akan otomatis membangun proyek dan menjalankan lexer pada file [examples/program_simple.pas](examples/program_simple.pas).

**Menjalankan secara manual:**

Anda juga bisa menjalankan file eksekusi `paslex` secara langsung:

    ./paslex examples/program_simple.pas


## Pembagian Tugas

| Nama | NIM | Tugas |
| --- | --- | --- |
| Raka Daffa Iftikhaar | 13523018 | Melakukan pengujian dan membuat kesimpulan pada laporan |
| Aliya Husna Fayyaza | 13523062 | Mengimplementasikan lexer, dfa_loader, dan membuat diagram DFA |
| Ahsan Malik Al Farisi | 13523074 | Membuat diagram DFA, penjelasan DFA pada laporan, dan membuat makefile |
| Bevinda Vivian | 13523120 | Membuat file README.md dan Landasan Teori pada laporan |