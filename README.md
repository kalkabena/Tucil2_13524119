# Tucil2_13524119

## Penjelasan

Repo ini terdiri dari dua program:  
**Voxellizer**: Menerima sebuah file .obj dengan face segitiga dan integer kedalaman maksimum dan mengeluarkan geometri yang diubah menjadi rangkaian voxel dalam bentuk .obj.  
**Objviewer**: Melihat sebuah file .obj dengan semua face segitiga. (Hanya untuk Windows)

## Requirement

Compiler C jika ingin kompilasi.

## Cara Kompilasi

Contoh Voxellizer: gcc -O3 -o Voxellizer main.c vertex.c face.c vertex_list.c face_list.c voxel_octree.c  
Contoh Objviewer: gcc -o Objviewer inputtest.c vertex.c face.c vertex_list.c face_list.c -mwindows

## Cara Menggunakan

**Voxellizer**  
1. Buka program
2. Input *path* absolut ke program
3. Input kedalaman maksimum

**Objviewer**  
1. Buka program
2. Buka file .obj
3. Navigasi dengan kontrol:
  - X/Z : +/- sudut x
  - S/A : +/- sudut y
  - W/Q : +/- sudut z
  - *Scrollwheel* : zoom
  - ESC : keluar program

## Identitas

Nathanael Shane Bennet, NIM 13524119 STEI-K ITB
