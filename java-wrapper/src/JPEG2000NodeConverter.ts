import * as fs from 'fs';
import * as path from 'path';
import { spawn } from 'child_process';

export class JPEG2000NodeConverter {
  private jarPath: string;

  constructor() {
    this.jarPath = path.join(__dirname, '..', '..', 'jpeg2000-converter', 'target', 'jpeg2000-converter-0.0.1.jar');
  }

  /**
   * Converts an image buffer to JPEG2000 format
   * @param inputBuffer Image data as a buffer
   * @returns Converted image as a buffer
   */
  async convertToJPEG2000(inputBuffer: Buffer): Promise<Buffer> {
    return this._convertImage(inputBuffer, 'jp2');
  }

  /**
   * Converts a JPEG2000 buffer to another format
   * @param inputBuffer JPEG2000 image as a buffer
   * @param format Target format (default: "png")
   * @returns Converted image as a buffer
   */
  async convertFromJPEG2000(inputBuffer: Buffer, format: string = 'png'): Promise<Buffer> {
    return this._convertImage(inputBuffer, format);
  }

  /**
   * Handles the conversion process by interacting with the Java JAR
   * @param inputBuffer The input image buffer
   * @param format Target format
   * @returns Converted image as a buffer
   */
  private async _convertImage(inputBuffer: Buffer, format: string): Promise<Buffer> {
    try {
      const base64Image = inputBuffer.toString('base64');
      const resultBase64 = await this._executeJar([base64Image, format]);

      return Buffer.from(resultBase64, 'base64');
    } catch (error) {
      console.error(`Error converting to ${format}:`, error);
      throw error;
    }
  }

  /**
   * Executes the Java JAR with provided arguments
   * @param args Arguments to pass to the JAR
   * @returns The process output as a Base64-encoded string
   */
  private _executeJar(args: string[]): Promise<string> {
    return new Promise((resolve, reject) => {
      const javaProcess = spawn('java', ['-jar', this.jarPath, ...args]);

      let output = '';
      let errorOutput = '';

      javaProcess.stdout.on('data', (data) => (output += data.toString()));
      javaProcess.stderr.on('data', (data) => (errorOutput += data.toString()));

      javaProcess.on('close', (code) => {
        if (code !== 0) {
          reject(new Error(`Java process failed with code ${code}: ${errorOutput}`));
        } else {
          resolve(output.trim()); // Ensure no extra newlines in Base64 output
        }
      });
    });
  }
}
