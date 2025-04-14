## 📖 Overview

This project provides a simple image converter written in C. It allows converting **JPEG 2000 (JP2)** images to **JPEG (JPG)** format using the OpenJPEG library.

---

## ⚙️ How It Works

- The core of the program is in `convert.c`, which performs the decoding of `.jp2` files and outputs them as `.jpg`.
- It uses the **OpenJPEG library** (`libopenjp2`) for handling JP2 decoding.
- A test file `test.jp2` is used to verify that the conversion works as expected.

---

## ✅ Requirements

To build and run this tool, you need:

- A **C compiler** (e.g., `gcc`)
- The **OpenJPEG** library (specifically `libopenjp2`)
- The **math** library (`libm`)
- A sample JP2 file for testing: `test.jp2` (must be in the same directory as the binary)

> Note: In the future, CMake will be used for managing builds. For now, builds are done manually.

---

## 🛠️ Compile

Run this command to compile the tool in **development mode** (adds debug symbols):

```bash
gcc -DDEBUG convert.c -o convert -static -lopenjp2 -lm
```

### Production Build for Node.js Integration

To compile and place the binary in the `node` library directory:

```bash
gcc convert.c -o ../lib/src/bin/convert -static -lopenjp2 -lm
```

> ⚠️ Use `-DDEBUG` **only** for development purposes.

---

## 🧪 Run Tests

To test the conversion:

```bash
cat test.jp2 | ./convert jpg > output.jpg
```

or simply:

```bash
./convert --test
```

> Make sure `test.jp2` is present in the **core** directory where you're running the test.
