![Orien Browser](./docs/images/orien.svg)

# Orien Browser

[![Build Orien Browser](https://github.com/wchadirin929-hub/Orien/actions/workflows/build-orien.yml/badge.svg)](https://github.com/wchadirin929-hub/Orien/actions/workflows/build-orien.yml)
[![License: MPL-2.0](https://img.shields.io/badge/License-MPL_2.0-blue.svg)](./LICENSE)
[![Platform: Windows x64](https://img.shields.io/badge/Platform-Windows%20x64%20(Portable)-success.svg)](https://github.com/wchadirin929-hub/Orien/releases)

**Orien Browser** adalah peramban web modern, cepat, ringan, dan mengutamakan privasi pengguna (*fast, lightweight, and private web browser*). Dibangun di atas fondasi Chromium dan Brave Core dengan kustomisasi identitas, performa, dan alur kerja portabel untuk platform Windows x64.

---

## 🌟 Fitur Utama (Key Features)

- 🛡️ **Privasi & Pemblokir Iklan Terintegrasi**: Dilengkapi mesin pemblokir iklan bawaan berbasis Rust (`adblock-rust`) yang secara otomatis memblokir iklan invasif, pelacak lintas situs, dan skrip berbahaya tanpa membutuhkan ekstensi pihak ketiga.
- 🚀 **Cepat & Ringan**: Dioptimalkan untuk kecepatan pemuatan halaman web maksimal dengan efisiensi penggunaan memori (RAM) dan CPU.
- 💼 **Mode Portabel Mandiri (Portable Launcher)**: Dilengkapi skrip `Buka-Orien-Portable.bat` yang secara otomatis mengarahkan data profil pengguna ke folder lokal `User_Data`, memungkinkan peramban dijalankan dari flashdisk atau drive eksternal tanpa mengotori registry sistem.
- ⚙️ **CI/CD Build Automation**: Workflow GitHub Actions mandiri dengan sistem *persistent Ninja build cache*, penyesuaian Windows SDK toolchain, dan pengemasan otomatis berkas ZIP siap pakai (`Orien-Browser-Portable-Windows-x64.zip`).
- 🔒 **Bebas Telemetri Mengganggu**: Menjaga data penjelajahan Anda tetap privat dan aman setiap saat.

---

## 🚀 Cara Menjalankan Orien (Portable)

Setelah mengunduh atau mengkompilasi file biner, Anda dapat langsung menjalankan Orien Browser secara portabel:

Cukup klik ganda atau jalankan:
```cmd
Buka-Orien-Portable.bat
```

> **Catatan:** Skrip ini akan mencari file eksekusi `orien.exe` (atau `brave.exe`) di dalam folder `out\Release` atau di folder utama, lalu menjalankannya dengan parameter `--user-data-dir="%~dp0User_Data"` sehingga seluruh riwayat, bookmark, dan pengaturan tersimpan rapi secara lokal dan portabel.

---

## 🏗️ Arsitektur Proyek (Overview)

Repositori ini menyimpan *build tools*, konfigurasi, patch, dan kode sumber kustom yang digunakan untuk membangun Orien Browser:

- **[Chromium](https://chromium.googlesource.com/chromium/src.git)**:
  - Kode inti browser Chromium yang diunduh dan disinkronkan melalui Google `depot_tools`.
- **Orien Core (src/brave)**:
  - Dipasang pada direktori `src/brave`.
  - Berisi identitas merek Orien, penyesuaian UI/UX, tema, lokalisasi teks, dan patch untuk kode Chromium pihak ketiga.
- **[adblock-rust](https://github.com/brave/adblock-rust)**:
  - Mesin adblock berkinerja tinggi yang dihubungkan melalui FFI bindings.
- **Workflow Build Otomatis (`.github/workflows/build-orien.yml`)**:
  - Mengelola proses kompilasi rilis Windows x64 pada runner GitHub Actions dengan optimasi compiler Clang-cl dan cache Ninja.

---

## 📥 Unduhan & Rilis (Downloads)

Untuk mengunduh rilis siap pakai dalam format Portable ZIP:
- Kunjungi halaman [GitHub Releases](https://github.com/wchadirin929-hub/Orien/releases) atau periksa tab **Actions** pada [Artifacts build terbaru](https://github.com/wchadirin929-hub/Orien/actions/workflows/build-orien.yml).
- Unduh paket `Orien-Browser-Portable-Windows-x64.zip`, ekstrak ke folder pilihan Anda, dan jalankan `Buka-Orien.bat` atau `Buka-Orien-Portable.bat`.

---

## 🛠️ Prasyarat Pengembangan (Prerequisites)

Untuk melakukan kompilasi Orien Browser secara lokal di Windows:

1. **Sistem Operasi**: Windows 10/11 64-bit (aktifkan *Developer Mode* dan *Long Paths*).
2. **Visual Studio**: Visual Studio 2022 Community/Professional dengan paket:
   - *Desktop development with C++*
   - *Windows 10/11 SDK (10.0.22621.0 atau lebih baru)*
3. **Node.js & pnpm**:
   - Node.js versi 20+ atau 24 LTS
   - `pnpm` (versi 10.x atau 11.x)
4. **Python**: Python versi 3.11+
5. **Google depot_tools**: Unduh dan tambahkan `depot_tools` ke bagian teratas variabel lingkungan `PATH`.

---

## 💻 Panduan Kompilasi Lokal (Build Guide)

### 1. Clone Repositori
Repositori Orien harus diletakkan pada folder `src/brave` di dalam direktori kerja proyek Anda:

```bash
git clone https://github.com/wchadirin929-hub/Orien.git path-to-your-project-folder/src/brave
cd path-to-your-project-folder/src/brave
```

### 2. Inisialisasi & Sinkronisasi Kode Sumber
Proses inisialisasi akan mengunduh dependensi dan kode sumber Chromium:

```bash
# Install paket Node.js
pnpm install

# Inisialisasi repositori dan dependensi Chromium
pnpm run init
```

### 3. Kompilasi Browser

Untuk melakukan kompilasi rilis optimal (*Release build*):
```bash
pnpm run build Release
```

Untuk kompilasi pengembangan (*Component build*):
```bash
pnpm run build
```

Untuk kompilasi statis (*Statically linked build*):
```bash
pnpm run build Static
```

### 4. Menjalankan Hasil Build
Setelah proses kompilasi selesai, jalankan:
```bash
# Melalui script launcher portabel
Buka-Orien-Portable.bat

# Atau melalui pnpm
pnpm start Release
```

---

## 🔄 Pembaruan & Sinkronisasi (Sync & Update)

Untuk memperbarui kode sumber dependensi dan menerapkan ulang patch:

```bash
pnpm run sync [--force] [--init] [--delete_unused_deps]
```

| Parameter Flag | Penjelasan |
| --- | --- |
| `[tanpa flag]` | Memperbarui Chromium jika diperlukan dan menerapkan ulang file `.patch` yang telah dimodifikasi. |
| `--force` | Memaksa pembaruan *Chromium* dan kode sumber ke *remote commit* terbaru serta menerapkan ulang semua patch. |
| `--init` | Mengatur ulang dependensi sesuai konfigurasi awal di `package.json`. |
| `-D, --delete_unused_deps` | Menghapus dependensi lama yang sudah tidak lagi digunakan dari *working tree*. |

---

## 🤝 Berkontribusi (Contributing)

Kontribusi untuk perbaikan bug, penambahan fitur, peningkatan keamanan, maupun saran optimasi selalu terbuka:
1. *Fork* repositori ini: `https://github.com/wchadirin929-hub/Orien`
2. Buat branch fitur baru (`git checkout -b fitur/nama-fitur`)
3. Lakukan commit perubahan Anda (`git commit -m 'feat: menambahkan fitur X'`)
4. Dorong ke branch Anda (`git push origin fitur/nama-fitur`)
5. Buka sebuah **Pull Request**

---

## 📄 Lisensi (License)

Orien Browser didistribusikan di bawah lisensi **Mozilla Public License 2.0 (MPL-2.0)**. Lihat berkas [LICENSE](./LICENSE) untuk informasi hak cipta dan ketentuan penggunaan lebih lanjut.

---

<div align="center">
  <sub>Dibangun dengan ❤️ oleh <a href="https://github.com/wchadirin929-hub">wchadirin929-hub</a> bersama komunitas sumber terbuka.</sub>
</div>
