#!/bin/bash

SRC_DIR="src"
INCLUDE_DIR="include"
BIN_DIR="bin"
OUTPUT="$BIN_DIR/myprogram.exe"

# Buat folder output jika belum ada
mkdir -p "$BIN_DIR"

# Cari semua file .cpp dari src dan subfolder-nya
CPP_FILES=$(find "$SRC_DIR" -type f -name "*.cpp")

# Compile pakai MSVC
cl \
  /I"$INCLUDE_DIR" \
  $CPP_FILES \
  /Fo:"$BIN_DIR\\" \
  /Fe:"$OUTPUT" \
  /std:c++17
