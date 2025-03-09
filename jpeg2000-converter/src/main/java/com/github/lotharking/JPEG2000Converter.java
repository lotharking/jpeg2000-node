package com.github.lotharking;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.Arrays;
import java.util.Iterator;
import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.ImageWriter;
import io.quarkus.runtime.Quarkus;
import io.quarkus.runtime.annotations.QuarkusMain;

@QuarkusMain
public class JPEG2000Converter {

    /**
     * Converts an image from one format to another.
     * If the format is invalid, it throws an exception.
     *
     * @param imageBytes The image in byte[] format
     * @param outputFormat The desired output format (e.g., "jpg", "png")
     * @return The converted image in byte[] format
     */
    public static byte[] convertImageFormat(byte[] imageBytes, String outputFormat) {
        try {
            // Read the input image
            BufferedImage image = ImageIO.read(new ByteArrayInputStream(imageBytes));
            if (image == null) {
                throw new IOException("Failed to read image.");
            }

            // Validate if the requested output format is supported
            Iterator<ImageWriter> writers = ImageIO.getImageWritersByFormatName(outputFormat);
            if (!writers.hasNext()) {
                throw new IOException("Unsupported output format: " + outputFormat);
            }

            // Convert the image and write it to a byte array
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ImageIO.write(image, outputFormat, baos);
            return baos.toByteArray();

        } catch (Exception e) {
            e.printStackTrace();
            throw new RuntimeException("Image conversion failed: " + e.getMessage(), e);
        }
    }

    /**
     * Main method to handle execution from another process.
     * Expects raw byte array input via standard input and an output format as an argument.
     *
     * @param args Command-line arguments: <output-format>
     */
    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println("Usage: java -jar jpeg2000-converter.jar <output-format>");
            System.exit(1);
        }

        String outputFormat = args[0];

        try {
            // Read input bytes from standard input
            ByteArrayOutputStream buffer = new ByteArrayOutputStream();
            int nRead;
            byte[] data = new byte[16384];
            while ((nRead = System.in.read(data, 0, data.length)) != -1) {
                buffer.write(data, 0, nRead);
            }
            buffer.flush();
            byte[] imageBytes = buffer.toByteArray();

            // Convert the image format
            byte[] convertedBytes = convertImageFormat(imageBytes, outputFormat);

            // Write converted bytes to standard output
            System.out.write(convertedBytes);
        } catch (Exception e) {
            System.err.println("Error during conversion: " + e.getMessage());
            System.exit(1);
        }
    }
}
