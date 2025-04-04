import { spawn } from 'child_process'
import * as fs from 'fs'
import * as path from 'path'

function getJarPath(): string {
  const fileName = 'core-0.0.1-runner'
  let searchDir: string | undefined

  try {
    searchDir = path.dirname(require.resolve('jp2-to-image/package.json'))
  } catch {
    searchDir = undefined
  }

  while (searchDir) {
    const candidate = path.join(searchDir, 'bin', fileName)
    if (fs.existsSync(candidate)) {
      return candidate
    }

    const parent = path.dirname(searchDir)
    if (parent === searchDir) break
    searchDir = parent
  }

  const localFallback = path.join(__dirname, 'bin', fileName)
  if (fs.existsSync(localFallback)) {
    return localFallback
  }

  throw new Error(`Executable '${fileName}' not found in any bin/ directory.`)
}

/**
 * Converts an image buffer to JPEG2000 format
 * @param inputBuffer Image data as a buffer
 * @returns Converted image as a buffer
 */
export async function convertToJPEG2000(inputBuffer: Buffer): Promise<Buffer> {
  return convertImage(inputBuffer, 'jp2')
}

/**
 * Converts a JPEG2000 buffer to another format
 * @param inputBuffer JPEG2000 image as a buffer
 * @param format Target format (default: "png")
 * @returns Converted image as a buffer
 */
export async function convertFromJPEG2000(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
  return convertImage(inputBuffer, format)
}

/**
 * Handles the conversion process by interacting with the Java JAR
 * @param inputBuffer The input image buffer
 * @param format Target format
 * @returns Converted image as a buffer
 */
async function convertImage(inputBuffer: Buffer, format: string): Promise<Buffer> {
  try {
    const jarPath = getJarPath()
    return await executeJar(jarPath, inputBuffer, format)
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
function executeJar(jarPath: string, inputBuffer: Buffer, format: string): Promise<Buffer> {
  return new Promise((resolve, reject) => {
    const javaProcess = spawn(jarPath, [format])

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
        resolve(Buffer.concat(outputBuffer))
      }
    })

    // Write image buffer to Java process and close stdin
    javaProcess.stdin.write(inputBuffer)
    javaProcess.stdin.end()
  })
}
