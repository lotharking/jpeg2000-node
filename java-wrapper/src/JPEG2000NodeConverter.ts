import { spawn } from 'child_process';
import * as fs from 'fs';
import * as path from 'path';
import * as os from 'os';

export class JPEG2000NodeConverter {
  private jarPath: string;

  constructor() {
    this.jarPath = path.join(__dirname, '..', 'jpeg2000-converter', 'target', 'jpeg2000-converter-0.0.1.jar');

    // Check if the JAR file exists
    if (!fs.existsSync(this.jarPath)) {
      throw new Error(`JAR file not found: ${this.jarPath}`);
    }
  }

  /**
   * Runs the JAR with the given arguments
   * @param args Arguments to pass to Java
   * @returns Java process output as a Promise<string>
   */
  private _executeJar(args: string[]): Promise<string> {
    return new Promise((resolve, reject) => {
      const java = spawn('java', ['-jar', this.jarPath, ...args]);

      let output = '';
      let errorOutput = '';

      java.stdout.on('data', (data: Buffer) => {
        output += data.toString();
      });

      java.stderr.on('data', (data: Buffer) => {
        errorOutput += data.toString();
      });

      java.on('close', (code: number) => {
        if (code !== 0) {
          reject(new Error(`Java process failed with code ${code}: ${errorOutput}`));
        } else {
          resolve(output.trim());
        }
      });
    });
  }

  /**
   * Converts a buffer containing an image to JPEG2000 format
   * @param inputBuffer Image data as a buffer
   * @returns Converted image as a buffer
   */
  async convertToJPEG2000(inputBuffer: Buffer): Promise<Buffer> {
    const tempDir = os.tmpdir();
    const inputPath = path.join(tempDir, `input_${Date.now()}.png`);
    const outputPath = path.join(tempDir, `output_${Date.now()}.jp2`);

    try {
      // Write buffer to temporary file
      fs.writeFileSync(inputPath, inputBuffer);

      // Execute conversion
      const result = await this._executeJar(['to', inputPath, outputPath]);
      if (!result.includes('Conversion successful')) {
        throw new Error('Conversion failed');
      }

      // Read the converted file and return as buffer
      return fs.readFileSync(outputPath);
    } catch (error) {
      console.error('Error converting to JPEG2000:', error);
      throw error;
    } finally {
      // Cleanup temporary files
      fs.unlinkSync(inputPath);
      if (fs.existsSync(outputPath)) fs.unlinkSync(outputPath);
    }
  }

  /**
   * Converts a JPEG2000 buffer to another format
   * @param inputBuffer JPEG2000 image as a buffer
   * @param format Target format (default: "png")
   * @returns Converted image as a buffer
   */
  async convertFromJPEG2000(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
    const tempDir = os.tmpdir();
    const inputPath = path.join(tempDir, `input_${Date.now()}.jp2`);
    const outputPath = path.join(tempDir, `output_${Date.now()}.${format}`);

    try {
      // Write buffer to temporary file
      fs.writeFileSync(inputPath, inputBuffer);

      // Execute conversion
      const result = await this._executeJar(['from', inputPath, outputPath, format]);
      if (!result.includes('Conversion successful')) {
        throw new Error('Conversion failed');
      }

      // Read the converted file and return as buffer
      return fs.readFileSync(outputPath);
    } catch (error) {
      console.error('Error converting from JPEG2000:', error);
      throw error;
    } finally {
      // Cleanup temporary files
      fs.unlinkSync(inputPath);
      if (fs.existsSync(outputPath)) fs.unlinkSync(outputPath);
    }
  }
}

export default JPEG2000NodeConverter;
