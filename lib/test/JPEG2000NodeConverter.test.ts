import * as fs from 'fs';
import * as path from 'path';
import { JPEG2000NodeConverter } from '../src';

describe('JPEG2000NodeConverter', () => {
  let converter: JPEG2000NodeConverter;
  const inputFilePath = path.join(__dirname, 'test.jp2');

  beforeAll(() => {
    converter = new JPEG2000NodeConverter();
  });

  it('should convert test.jp2 to jpg', async () => {
    const inputBuffer = fs.readFileSync(inputFilePath);
    const outputBuffer = await converter.convertFromJPEG2000(inputBuffer, 'jpg');
    const outputBase64 = outputBuffer.toString('base64');

    expect(outputBase64).toMatch(/^([A-Za-z0-9+/=]+)$/);
    expect(outputBase64.length).toBeGreaterThan(0);
  });
});
