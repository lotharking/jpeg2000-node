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
java -version
```
If Java is not installed, download it from [Adoptium](https://adoptium.net/) or your preferred provider.

---

## Installation
### 1. Install the Node.js package
```sh
yarn add jp2-to-image
# or
npm install jp2-to-image
```

### 2. Ensure Java is available
If Java is not installed globally, set the `JAVA_HOME` path accordingly.

### 3. Import and use in TypeScript
```typescript
import { JPEG2000NodeConverter } from "jp2-to-image";

const converter = new JPEG2000NodeConverter();
converter.convert("input.jp2", "output.png")
  .then(() => console.log("Conversion successful!"))
  .catch((error) => console.error("Conversion failed:", error));
```

---

## Usage
### Convert a JP2 image
```typescript
converter.convert("input.jp2", "output.png")
  .then(() => console.log("Image converted successfully"))
  .catch(console.error);
```

### Convert with custom options
```typescript
converter.convert("input.jp2", "output.jpeg", { quality: 90 })
  .then(() => console.log("JPEG image created"))
  .catch(console.error);
```

---

## Docker Support
This library is optimized for Docker environments. Since Java is already present in most base images, no additional installation is required.

### Example Dockerfile
```dockerfile
FROM node:18
WORKDIR /app
COPY package.json yarn.lock ./
RUN yarn install
COPY . .
CMD ["node", "index.js"]
```

To build and run:
```sh
docker build -t jp2-to-image .
docker run --rm jp2-to-image
```

---

## Development
### Build the Java Component
If you modify the Java source code, rebuild the JAR file using Maven:
```sh
cd core
mvn clean package
```
The compiled JAR will be available in `target/`.

### Run Tests
```sh
yarn test
```

---

## License  

This project is **completely free for any use** and is licensed under the **MIT License**. You can find the full license text in the `LICENSE` file in this repository.  

### Dependencies and Third-Party Licenses  

This project wraps the **jai-imageio-jpeg2000** library, which is subject to the following licenses:  

- **BSD 3-Clause License (with an additional nuclear disclaimer)**  
  - Applies to the `com.github.jaiimageio.jpeg2000` package and build modifications (e.g., `pom.xml`, tests).  
  - Full details are available in [`LICENSE-Sun.txt`](https://github.com/jai-imageio/jai-imageio-jpeg2000/blob/master/LICENSE-Sun.txt).  

- **JJ2000 License**  
  - Applies to the `jj2000` package within the library.  
  - This license **is not compatible with the GNU Public License (GPL)**, and its compatibility with other open-source licenses is **uncertain**.  

### Important Notes  

- **License Compatibility**: Since the **JJ2000 License** has restrictions and may be incompatible with other open-source licenses, users should review it carefully before distribution or commercial use.  
- **User Responsibility**: Developers and users should ensure they comply with the terms of all relevant licenses.  
- **More Information**:  
  - [`LICENSE-Sun.txt`](https://github.com/jai-imageio/jai-imageio-jpeg2000/blob/master/LICENSE-Sun.txt)  
  - [`COPYRIGHT.md`](https://github.com/jai-imageio/jai-imageio-jpeg2000/blob/master/COPYRIGHT.md)  

By including this section in your README, you make it clear that your **wrapper is free for any use** while acknowledging the **license constraints of the underlying library**. Let me know if you need further refinements! 🚀