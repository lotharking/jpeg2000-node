import { spawn } from 'child_process'
import * as path from 'path'

export class JPEG2000NodeConverter {
  private binPath: string

  constructor() {
    const platform = process.platform
    let binFileName = 'convert' // base path

    if (platform === 'win32') binFileName = 'convert-win.exe'
    else if (platform === 'darwin') binFileName = 'convert-mac'
    else if (platform === 'linux') binFileName = 'convert-linux'
    else throw new Error(`Unsupported platform: ${platform}`)

    this.binPath = path.resolve(__dirname, 'bin', binFileName)
  }

  /**
   * Handles the conversion process
   * @param inputBuffer The input image buffer
   * @param format Target format
   * @returns Converted image as a buffer
   */
  async convertImage(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
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
