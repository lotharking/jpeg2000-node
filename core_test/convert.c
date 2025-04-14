#include <openjpeg-2.4/openjpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- OpenJPEG Callbacks ---
void error_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[ERROR] %s\n", msg);
}

void warning_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[WARNING] %s\n", msg);
}

void info_callback(const char *msg, void *client_data) {
    (void)client_data;
    fprintf(stderr, "[INFO] %s\n", msg);
}

// Callback function for stb_image_write to write to file
static void write_to_file(void *context, void *data, int size) {
    FILE *file = (FILE*)context;
    fwrite(data, 1, size, file);
    fflush(file); // Ensure data is written immediately
}

int main(int argc, char *argv[]) {
    const char* format = "png";  // Default format
    const char* temp_input = "temp_input.jp2";
    const char* temp_output = "temp_output.bin";

    if (argc > 1) {
        format = argv[1];
    }

    // Validate supported format
    if (strcmp(format, "png") != 0 &&
        strcmp(format, "bmp") != 0 &&
        strcmp(format, "jpg") != 0 &&
        strcmp(format, "jpeg") != 0) {
        fprintf(stderr, "Error: Unsupported format: %s\nSupported formats: png, bmp, jpg, jpeg\n", format);
        return 1;
    }
    
    // --- Save stdin to temporary file ---
    FILE* temp_in = fopen(temp_input, "wb");
    if (!temp_in) {
        fprintf(stderr, "Error: Could not create temporary input file\n");
        return 1;
    }

    unsigned char buffer[8192];
    size_t bytes_read;
    size_t total_bytes = 0;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
        fwrite(buffer, 1, bytes_read, temp_in);
        total_bytes += bytes_read;
    }
    
    fclose(temp_in);
    
    if (total_bytes == 0) {
        fprintf(stderr, "Error: No data received from stdin\n");
        remove(temp_input);
        return 1;
    }
    
    fprintf(stderr, "Read %zu bytes from stdin\n", total_bytes);

    // --- Initialize OpenJPEG ---
    opj_codec_t* codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) {
        fprintf(stderr, "Error: Could not create decoder\n");
        remove(temp_input);
        return 1;
    }

    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);
    opj_set_info_handler(codec, info_callback, NULL);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);
    parameters.decod_format = 0; // JP2 format

    if (!opj_setup_decoder(codec, &parameters)) {
        fprintf(stderr, "Error setting up decoder\n");
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    opj_stream_t* stream = opj_stream_create_default_file_stream(temp_input, OPJ_TRUE);
    if (!stream) {
        fprintf(stderr, "Error creating stream\n");
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    opj_image_t* image = NULL;
    if (!opj_read_header(stream, codec, &image)) {
        fprintf(stderr, "Error reading header\n");
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    if (!opj_decode(codec, stream, image)) {
        fprintf(stderr, "Error decoding image\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    if (!opj_end_decompress(codec, stream)) {
        fprintf(stderr, "Error ending decompression\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    opj_stream_destroy(stream);

    fprintf(stderr, "Successfully decoded JP2 image: %d x %d with %d components\n", 
            image->x1 - image->x0, image->y1 - image->y0, image->numcomps);

    // --- Validation and conversion to RGB ---
    if (image->numcomps < 3) {
        fprintf(stderr, "Error: Image must have at least 3 components (RGB)\n");
        opj_image_destroy(image);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    int width = image->comps[0].w;
    int height = image->comps[0].h;

    for (int i = 1; i < 3; i++) {
        if (image->comps[i].w != width || image->comps[i].h != height) {
            fprintf(stderr, "Error: Components have different dimensions\n");
            opj_image_destroy(image);
            opj_destroy_codec(codec);
            remove(temp_input);
            return 1;
        }
    }

    unsigned char* rgb = malloc(width * height * 3);
    if (!rgb) {
        fprintf(stderr, "Error: Could not allocate memory for RGB image\n");
        opj_image_destroy(image);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    int max_value = (1 << image->comps[0].prec) - 1;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            for (int c = 0; c < 3; c++) {
                int value = image->comps[c].data[index];
                if (image->comps[c].sgnd)
                    value += max_value / 2;
                value = (value * 255) / max_value;
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                rgb[index * 3 + c] = (unsigned char)value;
            }
        }
    }

    fprintf(stderr, "Converted image to RGB format\n");

    // Create temporary output file
    FILE* temp_out = fopen(temp_output, "wb");
    if (!temp_out) {
        fprintf(stderr, "Error: Could not create temporary output file\n");
        free(rgb);
        opj_image_destroy(image);
        opj_destroy_codec(codec);
        remove(temp_input);
        return 1;
    }

    // --- Write to temporary file using stb_image_write ---
    int result = 0;
    if (strcmp(format, "png") == 0) {
        result = stbi_write_png_to_func(write_to_file, temp_out, width, height, 3, rgb, width * 3);
    } else if (strcmp(format, "bmp") == 0) {
        result = stbi_write_bmp_to_func(write_to_file, temp_out, width, height, 3, rgb);
    } else if (strcmp(format, "jpg") == 0 || strcmp(format, "jpeg") == 0) {
        result = stbi_write_jpg_to_func(write_to_file, temp_out, width, height, 3, rgb, 90); // Quality 90
    }

    fclose(temp_out);

    if (!result) {
        fprintf(stderr, "Error writing image to temporary file\n");
        free(rgb);
        opj_image_destroy(image);
        opj_destroy_codec(codec);
        remove(temp_input);
        remove(temp_output);
        return 1;
    }

    fprintf(stderr, "Successfully encoded image to %s format\n", format);

    // --- Cleanup OpenJPEG resources ---
    free(rgb);
    opj_image_destroy(image);
    opj_destroy_codec(codec);

    // --- Copy temporary output file to stdout ---
    FILE* out_file = fopen(temp_output, "rb");
    if (!out_file) {
        fprintf(stderr, "Error: Could not open temporary output file for reading\n");
        remove(temp_input);
        remove(temp_output);
        return 1;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), out_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, stdout);
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Error: Failed to write all data to stdout\n");
            fclose(out_file);
            remove(temp_input);
            remove(temp_output);
            return 1;
        }
    }

    // Flush stdout to ensure all data is written
    fflush(stdout);
    fclose(out_file);

    // --- Remove temporary files ---
    remove(temp_input);
    remove(temp_output);

    fprintf(stderr, "Conversion complete - program will now exit\n");
    return 0;
}