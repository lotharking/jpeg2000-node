# JPEG2000 Node Wrapper

## Overview
JPEG2000 Node Wrapper is a simple and efficient solution for converting JP2 images in TypeScript-based systems. It provides a seamless interface between Node.js and a Java-based JPEG2000 converter, leveraging Java's widespread availability in most systems to avoid additional installations.

This project consists of two main components:
1. **jp2-to-image** (Node.js Wrapper) - A TypeScript library that exposes a user-friendly API to convert JP2 images.
2. **core** (Java Backend) - A Java-based converter that performs the actual JP2 image processing.

This approach ensures compatibility across various environments, including Docker, without requiring additional dependencies beyond Java itself.

---

## Features
✅ Easy-to-use TypeScript API for JP2 image conversion  
✅ Works out-of-the-box with Java installed  
✅ No need for extra dependencies in Docker environments  
✅ Reliable and scalable solution for image processing  
✅ Fully open-source and free for any use  

---

## Prerequisites
Before using JPEG2000 Node Wrapper, ensure you have the following installed:

- **Node.js** (version 16+ recommended)
- **Java** (JDK 11+ recommended)
- **Maven** (for building the Java component, if needed)

To check if Java is installed, run:
```sh
java --version
```
If Java is not installed, download it from [Adoptium](https://adoptium.net/) or your preferred provider.

---
## Development
### Build the Java Component
If you modify the Java source code, rebuild the JAR file using Maven:
```sh
cd core/
mvn clean package -Pnative
```
The compiled JAR will be available in `target/`.

### Run Tests
```sh
cd lib/
yarn test
```

---

## License  

This project is **completely free for any use** and is licensed under the **MIT License**. You can find the full license text in the `LICENSE` file in this repository.  

### Dependencies and Third-Party Licenses  

This project wraps the **OpenJPEG** library, which is licensed under the **MIT License**.  

- **MIT License**  
  - Applies to the `OpenJPEG` library used for JPEG 2000 image processing.  
  - Full details are available in the [`LICENSE`](https://github.com/uclouvain/openjpeg/blob/master/LICENSE) file of the OpenJPEG repository.  

### Important Notes  

- **License Compatibility**: Since OpenJPEG is released under the **MIT License**, it is highly compatible with other open-source and commercial licenses.  
- **User Responsibility**: Developers and users should ensure they comply with the terms of all relevant licenses.  
- **More Information**:  
  - [OpenJPEG Repository](https://github.com/uclouvain/openjpeg)  
  - [OpenJPEG License](https://github.com/uclouvain/openjpeg/blob/master/LICENSE)  

By using OpenJPEG, this project remains **fully MIT-licensed** without compatibility issues. 🚀  
