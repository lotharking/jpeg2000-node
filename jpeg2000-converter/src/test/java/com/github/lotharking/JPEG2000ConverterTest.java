package com.github.lotharking;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.*;

class JPEG2000ConverterTest {

    @TempDir
    Path tempDir;

    @Test
    void testConvertImageFormatToJPG() throws IOException {
        // 1. Load the test image from resources and copy it to a temporary file
        Path testImagePath = tempDir.resolve("test.jp2");
        try (InputStream resourceStream = getClass().getClassLoader().getResourceAsStream("test.jp2")) {
            assertNotNull(resourceStream, "The test.jp2 image does not exist in the resources directory");
            Files.copy(resourceStream, testImagePath);
        }

        // 2. Read the image as bytes
        byte[] imageBytes = Files.readAllBytes(testImagePath);
        assertNotNull(imageBytes, "Failed to read image as bytes");
        assertTrue(imageBytes.length > 0, "Image file is empty");

        // 3. Convert the image to JPG format using the updated method
        byte[] convertedBytes = JPEG2000Converter.convertImageFormat(imageBytes, "jpg");
        assertNotNull(convertedBytes, "Conversion to JPG failed and returned null");
        assertTrue(convertedBytes.length > 0, "Converted image is empty");

        // 4. Save the converted image in the temporary directory
        Path outputImagePath = tempDir.resolve("output.jpg");
        Files.write(outputImagePath, convertedBytes);

        // 5. Verify that the converted image exists
        assertTrue(Files.exists(outputImagePath), "The converted JPG file was not created");
    }
}
