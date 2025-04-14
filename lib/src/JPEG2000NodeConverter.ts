import { spawn } from 'child_process'
import * as fs from 'fs'
import * as path from 'path'

export class JPEG2000NodeConverter {
  private binPath: string

  constructor() {
    const localPath = path.join(__dirname, 'bin', 'convert')
    let modulePath: string | null = null

    try {
      const packageRoot = path.dirname(require.resolve('jp2-to-image/package.json')) // package name
      modulePath = path.join(packageRoot, 'bin', 'convert')
    } catch (error) {
      modulePath = null
    }

    if (modulePath && fs.existsSync(modulePath)) {
      this.binPath = modulePath
    } else {
      this.binPath = localPath
    }
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
   * Handles the conversion process by interacting with the Java JAR
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
   * Executes the Java JAR, passing the image buffer via stdin.
   * @param inputBuffer The image as a Buffer
   * @param format The desired output format (e.g., "jpg", "png")
   * @returns The converted image as a Buffer
   */
  private _executeJar(inputBuffer: Buffer, format: string): Promise<Buffer> {
    return new Promise((resolve, reject) => {
      const javaProcess = spawn(this.binPath, [format])

      const outputBuffer: Buffer[] = []
      let errorOutput = ''

      // Capture stdout as Buffer
      javaProcess.stdout.on('data', data => outputBuffer.push(data))

      // Capture stderr
      javaProcess.stderr.on('data', data => (errorOutput += data.toString()))

      javaProcess.on('close', code => {
        if (code !== 0) {
          reject(new Error(`Java process failed with code ${code}: ${errorOutput}`))
        } else {
          resolve(Buffer.concat(outputBuffer)) // Combine all received chunks
        }
      })

      // Write image buffer to Java process and close stdin
      javaProcess.stdin.write(inputBuffer)
      javaProcess.stdin.end()
    })
  }
}
