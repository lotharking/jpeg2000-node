import * as fs from 'fs'
import * as path from 'path'

import { JPEG2000NodeConverter } from '../src'

const FIXTURE = path.join(__dirname, '..', 'assets', 'test.jp2')

describe('JPEG2000NodeConverter', () => {
  let inputBuffer: Buffer

  beforeAll(() => {
    inputBuffer = fs.readFileSync(FIXTURE)
  })

  it('converts test.jp2 to jpg', async () => {
    const start = Date.now()
    const output = await new JPEG2000NodeConverter().convertImage(inputBuffer, 'jpg')
    console.log(`JP2 → JPG took ${Date.now() - start}ms`)

    expect(output.length).toBeGreaterThan(0)
    expect(output.toString('base64')).toMatch(/^([A-Za-z0-9+/=]+)$/)
    // JPEG magic bytes: FF D8 FF
    expect(output[0]).toBe(0xff)
    expect(output[1]).toBe(0xd8)
    expect(output[2]).toBe(0xff)
  })

  it('converts test.jp2 to png', async () => {
    const output = await new JPEG2000NodeConverter().convertImage(inputBuffer, 'png')

    expect(output.length).toBeGreaterThan(0)
    // PNG magic bytes: 89 50 4E 47
    expect(output[0]).toBe(0x89)
    expect(output[1]).toBe(0x50)
    expect(output[2]).toBe(0x4e)
    expect(output[3]).toBe(0x47)
  })

  it('handles concurrent conversions correctly', async () => {
    const results = await Promise.all(
      Array.from({ length: 10 }, () => new JPEG2000NodeConverter().convertImage(inputBuffer, 'png')),
    )

    // All results must be identical (regression for temp-file concurrency bug)
    const reference = results[0].toString('base64')
    for (const result of results) {
      expect(result.toString('base64')).toBe(reference)
    }
  })

  it('throws on unsupported format', async () => {
    await expect(new JPEG2000NodeConverter().convertImage(inputBuffer, 'bmp')).rejects.toThrow(
      /Unsupported format/,
    )
  })
})
