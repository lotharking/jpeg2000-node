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

## **License**  

MIT  
