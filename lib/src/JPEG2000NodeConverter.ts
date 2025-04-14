import { spawn } from 'child_process'
import * as path from 'path'

export class JPEG2000NodeConverter {
  private binPath: string

  constructor() {
    this.binPath = path.resolve(__dirname, 'bin', 'convert')
  }

  /**
   * Converts an image buffer to JPEG2000 format
   * @param inputBuffer Image data as a buffer
   * @returns Converted image as a buffer
   */
  async convertToJPEG2000(inputBuffer: Buffer): Promise<Buffer> {
    return this._convertImage(inputBuffer, 'jp2')
  }

  /**
   * Converts a JPEG2000 buffer to another format
   * @param inputBuffer JPEG2000 image as a buffer
   * @param format Target format (default: "png")
   * @returns Converted image as a buffer
   */
  async convertFromJPEG2000(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
    return this._convertImage(inputBuffer, format)
  }

  /**
   * Handles the conversion process
   * @param inputBuffer The input image buffer
   * @param format Target format
   * @returns Converted image as a buffer
   */
  private async _convertImage(inputBuffer: Buffer, format: string): Promise<Buffer> {
    try {
      const convertedBuffer = await this._executeJar(inputBuffer, format)
      return convertedBuffer
    } catch (error) {
      console.error(`Error converting to ${format}:`, error)
      throw error
    }
  }

  /**
   * Executes the conversion binary, sending the image via stdin and receiving the result via stdout.
   *
   * @param inputBuffer The input image as a Buffer
   * @param format The desired output format (e.g., "jpg", "png", "jp2")
   * @returns The converted image as a Buffer
   */
  private _executeJar(inputBuffer: Buffer, format: string): Promise<Buffer> {
    return new Promise((resolve, reject) => {
      const process = spawn(this.binPath, [format])

      const outputBuffer: Buffer[] = []
      let errorOutput = ''

      // Capture stdout as Buffer
      process.stdout.on('data', data => outputBuffer.push(data))

      // Capture stderr
      process.stderr.on('data', data => (errorOutput += data.toString()))

      process.on('close', code => {
        if (code !== 0) {
          reject(new Error(`Process failed with code ${code}: ${errorOutput}`))
        } else {
          resolve(Buffer.concat(outputBuffer)) // Combine all received chunks
        }
      })

      // Write image buffer to process and close stdin
      process.stdin.write(inputBuffer)
      process.stdin.end()
    })
  }
}
