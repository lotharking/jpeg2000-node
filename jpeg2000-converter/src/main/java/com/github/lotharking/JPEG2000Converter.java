package com.github.lotharking;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.Iterator;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriter;

public class JPEG2000Converter {

    /**
     * Converts a byte[] image from one format to another.
     * If the format is invalid, it returns the same input.
     *
     * @param imageBytes The image in byte[] format
     * @param outputFormat The desired output format (e.g., "jpg", "png")
     * @return The image in byte[] format in the specified output format, or null if an error occurs
     */
    public static byte[] convertImageFormat(byte[] imageBytes, String outputFormat) {
        try {
            // Read the image
            BufferedImage image = ImageIO.read(new ByteArrayInputStream(imageBytes));
            if (image == null) {
                throw new IOException("Failed to read image. Ensure that the format is supported.");
            }

            // Validate output format
            Iterator<ImageWriter> writers = ImageIO.getImageWritersByFormatName(outputFormat);
            if (!writers.hasNext()) {
                throw new IOException("Unsupported output format: " + outputFormat);
            }

            // Convert and write image
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ImageIO.write(image, outputFormat, baos);
            return baos.toByteArray();

        } catch (IOException e) {
            throw new RuntimeException("Conversion failed", e);
        }
    }
}
