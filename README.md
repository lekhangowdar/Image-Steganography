# LSB Image Steganography

## 📌 Project Overview
This project implements **Least Significant Bit (LSB) based Image Steganography**, which is the technique of hiding secret data within an image file by modifying the least significant bits of pixel values.
The process ensures that the image remains visually unchanged while carrying embedded information.

Both sender and receiver share a secret key to encode/decode the hidden data, enhancing security. The project also serves as a hands-on introduction to basic **image processing** techniques.

---

## 🔧 Features
- Hide text data inside image files (e.g., `.bmp`)
- Extract hidden data using a shared secret key
- Preserve image integrity with minimal visual distortion
- Learn bitwise operations and file manipulation in C
- Understand how data is represented and manipulated at the byte/bit level

---

## 🧠 Pre-requisites
Before diving in, ensure you're comfortable with:
- Function pointers
- File I/O operations
- Bitwise operations

---

## 🚀 How It Works
1. **Encoding**:
   - Take a source image and the message to hide.
   - Modify the LSBs of pixel data based on the message bits.
   - Save the modified image as a stego-image.

2. **Decoding**:
   - Read the stego-image using the shared key.
   - Extract and reconstruct the original hidden message from LSBs.

---

## 🛠️ Tech Stack
- Language: **C**
- Image format: **.bmp (Bitmap)**
- Tools: GCC compiler, Makefile (optional)

---
