package com.github.lotharking;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.Iterator;
import javax.imageio.ImageIO;
import javax.imageio.ImageWriter;
import javax.imageio.spi.IIORegistry;

import com.github.jaiimageio.jpeg2000.impl.J2KImageReaderSpi;
import com.github.jaiimageio.jpeg2000.impl.J2KImageWriterSpi;

import io.quarkus.runtime.annotations.QuarkusMain;
import jakarta.annotation.PostConstruct;

@QuarkusMain
public class JPEG2000Converter {
    @PostConstruct
    public void initializeImageIO() {
        IIORegistry registry = IIORegistry.getDefaultInstance();
        registry.registerServiceProvider(new J2KImageReaderSpi());
        registry.registerServiceProvider(new J2KImageWriterSpi());
    }

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
            System.err.println("Usage: java -jar core.jar <output-format>");
            System.exit(1);
        }

        String outputFormat = args[0];

        try (BufferedInputStream bis = new BufferedInputStream(System.in);
             ByteArrayOutputStream buffer = new ByteArrayOutputStream();
             BufferedOutputStream bos = new BufferedOutputStream(System.out)) {

            byte[] data = new byte[65536];
            int nRead;
            while ((nRead = bis.read(data)) != -1) {
                buffer.write(data, 0, nRead);
            }
            byte[] imageBytes = buffer.toByteArray();

            byte[] convertedBytes = convertImageFormat(imageBytes, outputFormat);
            bos.write(convertedBytes);
            bos.flush();
        } catch (Exception e) {
            System.err.println("Error during conversion: " + e.getMessage());
            System.exit(1);
        }

    }
}
