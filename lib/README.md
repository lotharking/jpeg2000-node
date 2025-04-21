## Overview

`jp2-to-image` is a powerful open-source library for image conversion, specifically designed to handle JPEG 2000 (JP2) format seamlessly. It provides a simple API to convert JP2 images into widely supported formats like PNG or JPEG while delivering the output as a Base64 string. This ensures smooth integration into your workflow, allowing you to process images without limitations or bottlenecks. 🚀

## Installation

### 1. Install the Node.js package

```sh
yarn add jp2-to-image
# or
npm install jp2-to-image
# or
pnpm add jp2-to-image
```

### 2. Import and use in TypeScript

```typescript
import { convertFromJPEG2000 } from 'jp2-to-image'

try {
  await new JPEG2000NodeConverter().convertFromJPEG2000(inputBuffer, 'png') // Return a buffer
  console.log('Conversion successful!')
} catch (error) {
  console.error('Conversion failed:', error)
}
```
