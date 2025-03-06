import * as fs from 'fs';
import * as path from 'path';
import { JPEG2000NodeConverter } from '../src';

describe('JPEG2000NodeConverter', () => {
  let converter: JPEG2000NodeConverter;
  const inputFilePath = path.join(__dirname, 'test.jp2');

  beforeAll(() => {
    converter = new JPEG2000NodeConverter();
  });

  it('should convert test.jp2 to jpg as base64', async () => {
    const inputBuffer = fs.readFileSync(inputFilePath);
    const inputBase64 = inputBuffer.toString('base64');

    const outputBuffer = await converter.convertFromJPEG2000(Buffer.from(inputBase64, 'base64'), 'jpg');
    const outputBase64 = outputBuffer.toString('base64');

    // Validación: la salida debe ser un Base64 válido
    expect(outputBase64).toMatch(/^([A-Za-z0-9+/=]+)$/);
    expect(outputBase64.length).toBeGreaterThan(0);
  });
});
