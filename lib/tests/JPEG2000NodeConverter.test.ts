import * as fs from 'fs'
import * as path from 'path'

import { JPEG2000NodeConverter } from '../src'

describe('JPEG2000NodeConverter', () => {
  const inputFilePath = path.join(__dirname, '..', 'assets', 'test.jp2')

  it('should convert test.jp2 to jpg', async () => {
    const inputBuffer = fs.readFileSync(inputFilePath)

    const start = Date.now()
    const outputBuffer = await new JPEG2000NodeConverter().convertImage(inputBuffer, 'jpg')
    const duration = Date.now() - start

    console.log(`Conversion took ${duration}ms`)

    const outputBase64 = outputBuffer.toString('base64')
    expect(outputBase64).toMatch(/^([A-Za-z0-9+/=]+)$/)
    expect(outputBase64.length).toBeGreaterThan(0)
  }, 15000)
})
