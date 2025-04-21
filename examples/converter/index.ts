import * as fs from 'fs'
import * as path from 'path'
import { JPEG2000NodeConverter } from 'jp2-to-image'

async function main() {
  const inputFilePath = path.join(__dirname, 'test.jp2')
  const outputFilePath = path.join(__dirname, 'output.jpg')

  if (!fs.existsSync(inputFilePath)) {
    console.error(`File not found: ${inputFilePath}`)
    process.exit(1)
  }

  const inputBuffer = fs.readFileSync(inputFilePath)

  console.log('Starting conversion...')
  const start = Date.now()

  try {
    const outputBuffer = await new JPEG2000NodeConverter().convertImage(inputBuffer, 'jpg')
    const duration = Date.now() - start

    fs.writeFileSync(outputFilePath, outputBuffer)

    console.log(`✅ Conversion completed in ${duration}ms`)
    console.log(`File saved at: ${outputFilePath}`)
  } catch (error) {
    console.error('❌ Error during conversion:', error)
  }
}

main()
