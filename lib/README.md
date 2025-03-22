## Overview

`jp2-to-image` is a powerful open-source library for image conversion, specifically designed to handle JPEG 2000 (JP2) format seamlessly. It provides a simple API to convert JP2 images into widely supported formats like PNG or JPEG while delivering the output as a Base64 string. This ensures smooth integration into your workflow, allowing you to process images without limitations or bottlenecks. 🚀

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
import { JPEG2000NodeConverter } from 'jp2-to-image'

const converter = new JPEG2000NodeConverter()
try {
  await converter.convert(inputBuffer, 'png')
  console.log('Conversion successful!')
} catch (error) {
  console.error('Conversion failed:', error)
}
```
