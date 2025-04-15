# JPEG2000 Node Wrapper

## Overview
**JPEG2000 Node Wrapper** is a simple and efficient solution for converting JP2 images in TypeScript-based systems. It provides a seamless interface between Node.js and a C-based JPEG2000 converter, leveraging the performance and portability of native C code without the need for Java or other heavy runtimes.

This project consists of two main components:
1. **jp2-to-image** (Node.js Wrapper) - A TypeScript library that exposes a user-friendly API to convert JP2 images.
2. **core** (C Backend) - A C-based converter that performs the actual JP2 image processing using the OpenJPEG library.

This approach ensures compatibility across various environments, including Docker, without requiring additional heavyweight dependencies.

---

## Features
✅ Easy-to-use TypeScript API for JP2 image conversion  
✅ Native performance using C and OpenJPEG  
✅ No Java or JVM required  
✅ Lightweight and production-ready for Docker environments  
✅ Reliable and scalable solution for image processing  
✅ Fully open-source and free for any use  

---

## Prerequisites
Before using JPEG2000 Node Wrapper, ensure you have the following installed:

- **Node.js** (version 16+ recommended)
- **A C compiler** (e.g., `gcc` or `clang`)  
- **OpenJPEG development libraries**

To install OpenJPEG on Ubuntu/Debian-based systems:
```sh
sudo apt-get update
sudo apt-get install libopenjp2-7-dev
```

To check if the CLI converter is available:
```sh
opj_decompress -h
```

Or if you're compiling your own C binary, make sure the `core/` folder contains the proper build scripts (`Makefile`, etc).

---

## Development

### Build the C Component
If you modify the C source code, rebuild the binary:
```sh
cd core/
make
```
This will produce the executable used by the Node.js wrapper.

### Run Tests
```sh
cd lib/
yarn test
```

---

## License  

This project is **completely free for any use** and is licensed under the **MIT License**. You can find the full license text in the `LICENSE` file in this repository.  

### Dependencies and Third-Party Licenses  

This project wraps the **OpenJPEG** library, which is licensed under the **BSD License**.  

### Important Notes  

By using OpenJPEG and C, this project remains **lightweight**, **performant**, and **fully MIT-licensed**. 🚀  
