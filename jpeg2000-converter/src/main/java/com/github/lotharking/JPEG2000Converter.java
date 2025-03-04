package com.github.lotharking;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.Base64;
import javax.imageio.ImageIO;

public class JPEG2000Converter {

    /**
     * Converts a base64 image from one format to another.
     * If the format is invalid, it returns the same input.
     *
     * @param base64Image The image in base64 format
     * @param outputFormat The desired output format (e.g., "jpg", "png")
     * @return The image in base64 format in the specified output format, or the same input if the format is invalid
     */
    public static String convertImageFormat(String base64Image, String outputFormat) {
        try {
            // Decode the base64 image to bytes
            byte[] imageBytes = Base64.getDecoder().decode(base64Image);

            // Read the image
            BufferedImage image = ImageIO.read(new ByteArrayInputStream(imageBytes));
            if (image == null) {
                throw new IOException("Failed to read image");
            }

            // Validate the output format
            if (!ImageIO.getImageWritersByFormatName(outputFormat).hasNext()) {
                return base64Image; // Return the same image if the format is invalid
            }

            // Convert the image to the desired format
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            ImageIO.write(image, outputFormat, baos);
            byte[] outputImageBytes = baos.toByteArray();

            // Encode the image back to base64
            return Base64.getEncoder().encodeToString(outputImageBytes);
        } catch (IOException e) {
            e.printStackTrace();
            return null; // Error handling: return null in case of failure
        }
    }
}

