[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ZCO_vauY)
Repository for final lesson before mid term of computer graphic course

Nama : Arnando Michael Gtg
NIM : 1313623014

# Mid Term Report: Segment Detection Algorithm

**Nama:** Armando Micahel Gtg
**NIM:** (1313623014)

---

## I. Objectives 1 & 2: Window Size Analysis and Pattern Fitting

### A. Window Size Analysis

Berdasarkan instruksi dan prinsip dasar pemrosesan citra digital untuk deteksi fitur garis dan sudut, **ukuran *window* yang benar dan paling sesuai** untuk mengidentifikasi segmen dasar adalah 3x3 (tiga kali tiga) piksel**.

**Alasan Pemilihan 3x3 :
1.  **Minimalitas:** Ukuran $3 \times 3$ adalah ukuran matriks terkecil yang memungkinkan representasi fitur-fitur garis dasar (*single pixel*, horizontal, vertikal, diagonal) dan sudut (L-shape/elbow).
2.  **Efisiensi:** Menggunakan *window* yang lebih besar (misalnya, $5 \times 5$) akan meningkatkan kompleksitas komputasi secara signifikan dan memperkenalkan lebih banyak variasi yang tidak diperlukan untuk mendeteksi segmen **dasar** tunggal.
3.  **Implementasi:** Kode `segmentDetection()` mengiterasi setiap piksel di kanvas dan mengekstrak matriks $3 \times 3$ di sekitarnya untuk dianalisis, yang secara struktural mendukung Objective 1.

### B. Segment Filter Patterns (Filter $3 \times 3$ Ideal)

Setelah menentukan ukuran *window* yang benar ($3 \times 3$), langkah selanjutnya adalah mendefinisikan pola (filter) yang sesuai untuk mengidentifikasi berbagai jenis segmen. Pola-pola ini didefinisikan dalam fungsi `getSegmentFilters()` dan berfungsi sebagai basis untuk Objective 3.

**Tabel Pola Filter Ideal ($1$ = *Drawn Pixel*, $0$ = *Background*)**

| No. | Jenis Segmen | Pola Matriks $3 \times 3$ | Keterangan |
| :---: | :---: | :---: | :--- |
| 1. | **Single Pixel** | `{{0,0,0}, {0,1,0}, {0,0,0}}` | Titik tunggal di tengah (pusat) |
| 2. | **Horizontal (3px)** | `{{0,0,0}, {1,1,1}, {0,0,0}}` | Garis horizontal penuh |
| 3. | **Vertical (3px)** | `{{0,1,0}, {0,1,0}, {0,1,0}}` | Garis vertikal penuh |
| 4. | **Diagonal Utama** | `{{1,0,0}, {0,1,0}, {0,0,1}}` | Diagonal dari kiri atas ke kanan bawah |
| 5. | **Anti-Diagonal** | `{{0,0,1}, {0,1,0}, {1,0,0}}` | Diagonal dari kanan atas ke kiri bawah |
| 6. | **Corner/Elbow 1** | `{{1,1,0}, {1,1,0}, {0,0,0}}` | Sudut (L-Shape) |
| 7. | **Corner/Elbow 2** | `{{1,1,0}, {1,0,0}, {0,0,0}}` | Sudut tajam |
| 8. | **Horizontal (2px)** | `{{0,0,0}, {1,1,0}, {0,0,0}}` | Segmen horizontal 2 piksel (untuk garis tipis) |
| 9. | **Vertical (2px)** | `{{0,1,0}, {0,1,0}, {0,0,0}}` | Segmen vertikal 2 piksel (untuk garis tipis) |

Pola-pola ini mewakili himpunan dasar segmen yang harus dicari dalam citra.

---

## II. Objective 3: Automated Segment Detection Algorithm

### A. Metode Implementasi

Objective 3 diimplementasikan dalam fungsi `DrawingCanvas::segmentDetection()`. Algoritma yang digunakan adalah **Convolutional Pattern Matching** sederhana:

1.  **Pengambilan Citra:** Ambil gambar kanvas saat ini (`QPixmap pixmap = this->grab();`).
2.  **Iterasi Jendela:** Iterasi melalui setiap piksel $P(i, j)$ di kanvas (dengan margin 1 piksel di sekitar batas).
3.  **Ekstraksi Matriks $3 \times 3$:** Pada setiap $P(i, j)$, sebuah matriks *window* $3 \times 3$ diekstrak, mencatat apakah piksel tersebut terisi (merah atau biru) atau tidak (putih).
4.  **Saringan *Non-Empty* (Objectives 1 & 2):** Hanya *window* yang memiliki setidaknya satu piksel terisi (`is_non_empty = true`) yang diproses lebih lanjut. Total *window* yang tidak kosong ini dihitung dan dicetak ke konsol (`cout`) untuk keperluan pelaporan (*dumping* data).
5.  **Pattern Matching:** Matriks *window* yang diekstrak dibandingkan dengan 9 *filter* ideal yang didefinisikan dalam `getSegmentFilters()` menggunakan operator *overload* `CustomMatrix::operator==`.
6.  **Kandidat Segmen:** Jika *window* $3 \times 3$ cocok dengan salah satu filter, koordinat pusat $(i, j)$ disimpan ke `m_segmentCandidates`.
7.  **Visualisasi:** Setelah pemindaian selesai, `paintEvent` dipicu. Titik-titik kandidat di `m_segmentCandidates` divisualisasikan sebagai **kotak kecil berwarna ungu (*darkMagenta*)** di atas kanvas, memverifikasi hasil deteksi.

### B. Debugging dan Proses Pengembangan dengan Gemini AI

Selama pengembangan kode, terjadi beberapa *error* yang memerlukan debugging dan perbaikan. Bantuan Gemini AI digunakan untuk memecahkan masalah kompilasi dan menyempurnakan logika:

| Masalah yang Dihadapi | Penyebab | Solusi yang Diterapkan |
| :--- | :--- | :--- |
| **Error Kompilasi:** `is_filled was not declared in this scope` | Variabel `is_filled` dan `is_non_empty` dideklarasikan di luar *scope* yang benar atau tidak dideklarasikan sama sekali dalam `segmentDetection()`. | **Deklarasi Variabel Lokal:** Variabel `bool is_non_empty` dan `bool is_filled` dideklarasikan di dalam *loop* `for` yang benar dalam `segmentDetection()` untuk mengatasi *error* *scope*. |
| **Error Kompilasi:** `Extended character is not valid in an identifier` | Karakter Unicode non-ASCII disalin/ditempelkan sebagai spasi atau tab, yang tidak dikenali oleh kompilator Qt/C++. | **Pembersihan Kode:** Semua spasi dan tab yang tersembunyi diganti dengan spasi ASCII standar di `drawingcanvas.cpp`. |
| **Penyempurnaan Logika:** Memastikan Objective 1 & 2 terpenuhi (dumping data). | Kode awal hanya fokus pada Objective 3 (deteksi), mengabaikan kebutuhan untuk menghitung dan menganalisis semua *window* yang tidak kosong. | **Implementasi Penghitungan *Window***: Menambahkan *counter* `non_empty_window_count` dan memastikan logika *dumping* (pencetakan matriks *window* ke konsol) ada dalam kode, sesuai instruksi. |
| **Visibilitas Hasil:** Hasil deteksi tidak ditampilkan secara jelas. | Tidak ada visualisasi untuk `m_segmentCandidates` pada `paintEvent`. | **Visualisasi Ungu:** Menambahkan logika di `paintEvent` untuk menggambar kotak ungu 4x4 di lokasi kandidat segmen. |

Proses ini memastikan kode akhir tidak hanya berjalan tanpa *error* kompilasi tetapi juga secara logis memenuhi semua Objectives yang diminta dalam tugas.
