import type { OpenjpegModule } from './wasm/openjp2'

import * as jpeg from 'jpeg-js'
import { PNG } from 'pngjs'

export class JPEG2000NodeConverter {
  private static modulePromise: Promise<OpenjpegModule> | null = null

  private static getModule(): Promise<OpenjpegModule> {
    if (!JPEG2000NodeConverter.modulePromise) {
      // eslint-disable-next-line @typescript-eslint/no-require-imports
      const createModule = require('./wasm/openjp2') as (
        options?: Record<string, unknown>,
      ) => Promise<OpenjpegModule>
      JPEG2000NodeConverter.modulePromise = createModule()
    }
    return JPEG2000NodeConverter.modulePromise
  }

  async convertImage(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
    const mod = await JPEG2000NodeConverter.getModule()
    return this._convert(mod, inputBuffer, format)
  }

  private _convert(mod: OpenjpegModule, inputBuffer: Buffer, format: string): Buffer {
    const inputPtr = mod._malloc(inputBuffer.length)
    if (!inputPtr) throw new Error('WASM memory allocation failed for input')

    const widthPtr = mod._malloc(4)
    const heightPtr = mod._malloc(4)
    const channelsPtr = mod._malloc(4)

    try {
      mod.HEAPU8.set(inputBuffer, inputPtr)

      const pixelsPtr = mod._decode_jp2(inputPtr, inputBuffer.length, widthPtr, heightPtr, channelsPtr)
      if (!pixelsPtr) throw new Error('JP2 decoding failed: invalid or unsupported image')

      const width = mod.getValue(widthPtr, 'i32')
      const height = mod.getValue(heightPtr, 'i32')
      const channels = mod.getValue(channelsPtr, 'i32')

      const pixelCount = width * height * channels
      const pixels = Buffer.from(mod.HEAPU8.buffer, pixelsPtr, pixelCount)
      // Copy before freeing WASM memory
      const pixelsCopy = Buffer.from(pixels)
      mod._free_buffer(pixelsPtr)

      return this._encode(pixelsCopy, width, height, channels, format)
    } finally {
      mod._free(inputPtr)
      mod._free(widthPtr)
      mod._free(heightPtr)
      mod._free(channelsPtr)
    }
  }

  private _encode(pixels: Buffer, width: number, height: number, channels: number, format: string): Buffer {
    const fmt = format.toLowerCase()
    const rgba = channels === 4 ? pixels : this._rgbToRgba(pixels, width, height)

    if (fmt === 'png') {
      const png = new PNG({ width, height })
      png.data = rgba
      return PNG.sync.write(png)
    }

    if (fmt === 'jpg' || fmt === 'jpeg') {
      const result = jpeg.encode({ data: rgba, width, height }, 90)
      return result.data as Buffer
    }

    throw new Error(`Unsupported format: "${format}". Supported formats: png, jpg, jpeg`)
  }

  private _rgbToRgba(rgb: Buffer, width: number, height: number): Buffer {
    const rgba = Buffer.alloc(width * height * 4)
    for (let i = 0; i < width * height; i++) {
      rgba[i * 4] = rgb[i * 3]
      rgba[i * 4 + 1] = rgb[i * 3 + 1]
      rgba[i * 4 + 2] = rgb[i * 3 + 2]
      rgba[i * 4 + 3] = 255
    }
    return rgba
  }
}
