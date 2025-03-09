# **jpeg2000-node**  

A Node.js wrapper for the Java library [jai-imageio-jpeg2000](https://github.com/jai-imageio/jai-imageio-jpeg2000), enabling image conversion to and from the JPEG2000 format.  

## **Requirements**  

- Node.js (v18 or later)  
- Java JDK (v17 or later)  

## **Installation**  

```bash
npm install jpeg2000-node
```

## **Usage**  

```javascript
const JPEG2000NodeConverter = require('jpeg2000-node');

// Create an instance of the converter
const converter = new JPEG2000NodeConverter();

// Convert an image to JPEG2000 format
async function convertToJP2() {
  const success = await converter.convertToJPEG2000('input.png', 'output.jp2');
  console.log(success ? 'Conversion successful' : 'Conversion failed');
}

// Convert a JPEG2000 image to another format
async function convertFromJP2() {
  const success = await converter.convertFromJPEG2000('input.jp2', 'output.png', 'png');
  console.log(success ? 'Conversion successful' : 'Conversion failed');
}

convertToJP2();
convertFromJP2();
```

## **API**  

### `new JPEG2000NodeConverter()`

Creates a new instance of the converter.

### `convertToJPEG2000(inputPath, outputPath)`

Converts an image to JPEG2000 format.  

- `inputPath`: Path to the input image file (can be any format supported by Java ImageIO)  
- `outputPath`: Path to the output JPEG2000 file  

Returns a promise that resolves to `true` if the conversion is successful.  

### `convertFromJPEG2000(inputPath, outputPath, format = 'png')`

Converts a JPEG2000 image to another format.  

- `inputPath`: Path to the input JPEG2000 file  
- `outputPath`: Path to the output image file  
- `format`: Output format (default: "png")  

Returns a promise that resolves to `true` if the conversion is successful.  

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