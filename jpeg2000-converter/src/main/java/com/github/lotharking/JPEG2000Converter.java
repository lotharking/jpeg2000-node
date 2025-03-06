package com.github.lotharking;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.Base64;
import java.util.Iterator;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriter;

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
                throw new IOException("Failed to read image. Ensure that the format is supported.");
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

        } catch (IOException e) {
            throw new RuntimeException("Image conversion failed", e);
        }
    }

    /**
     * Main method to handle command-line execution.
     * Expects a Base64-encoded image and an output format as arguments.
     *
     * @param args Command-line arguments: <base64-image> <output-format>
     */
    public static void main(String[] args) {
        if (args.length < 2) {
            System.err.println("Usage: java -jar jpeg2000-converter.jar <base64-image> <output-format>");
            System.exit(1);
        }

        String base64Image = args[0];
        String outputFormat = args[1];

        try {
            // Decode the Base64-encoded image
            byte[] imageBytes = Base64.getDecoder().decode(base64Image);

            // Convert the image format
            byte[] convertedBytes = convertImageFormat(imageBytes, outputFormat);

            // Print the converted image as a Base64-encoded string
            System.out.println(Base64.getEncoder().encodeToString(convertedBytes));
        } catch (Exception e) {
            System.err.println("Error during conversion: " + e.getMessage());
            System.exit(1);
        }
    }
}
