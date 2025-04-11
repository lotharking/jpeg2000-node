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
    fprintf(stdout, "[INFO] %s\n", msg);
}

int main(int argc, char *argv[]) {
    const char* input_filename = "test.jp2";
    std::string output_filename = "output.jpg";
    const char* format = "jpg";  // Default format

    if (argc > 1) input_filename = argv[1];
    if (argc > 2) {
        format = argv[2];
        output_filename = "output." + std::string(argv[2]);
    }

    // Validate supported format
    if (strcmp(format, "png") != 0 &&
        strcmp(format, "bmp") != 0 &&
        strcmp(format, "jpg") != 0 &&
        strcmp(format, "jpeg") != 0) {
        fprintf(stderr, "Error: Unsupported format: %s\nSupported formats: png, bmp, jpg\n", format);
        return 1;
    }
    
    // --- Read JP2 file into memory ---
    FILE* f = fopen(input_filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open file: %s\n", input_filename);
        return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Error: Could not allocate memory for buffer\n");
        fclose(f);
        return 1;
    }

    size_t read_bytes = fread(buffer, 1, size, f);
    fclose(f);
    if (read_bytes != size) {
        fprintf(stderr, "Error: Could not read the entire file\n");
        free(buffer);
        return 1;
    }

    // --- Initialize OpenJPEG ---
    opj_codec_t* codec = opj_create_decompress(OPJ_CODEC_JP2);
    if (!codec) {
        fprintf(stderr, "Error: Could not create decoder\n");
        free(buffer);
        return 1;
    }

    opj_set_error_handler(codec, error_callback, NULL);
    opj_set_warning_handler(codec, warning_callback, NULL);
    opj_set_info_handler(codec, info_callback, NULL);

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);

    if (!opj_setup_decoder(codec, &parameters)) {
        fprintf(stderr, "Error setting up decoder\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    opj_stream_t* stream = opj_stream_create_default_file_stream(input_filename, OPJ_TRUE);
    if (!stream) {
        fprintf(stderr, "Error creating stream\n");
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    opj_image_t* image = NULL;
    if (!opj_read_header(stream, codec, &image) ||
        !opj_decode(codec, stream, image) ||
        !opj_end_decompress(codec, stream)) {
        fprintf(stderr, "Error processing image\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    // --- Validation and conversion to RGB ---
    if (image->numcomps < 3) {
        fprintf(stderr, "Error: Image must have at least 3 components (RGB)\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
        return 1;
    }

    int width = image->comps[0].w;
    int height = image->comps[0].h;

    for (int i = 1; i < 3; i++) {
        if (image->comps[i].w != width || image->comps[i].h != height) {
            fprintf(stderr, "Error: Components have different dimensions\n");
            opj_image_destroy(image);
            opj_stream_destroy(stream);
            opj_destroy_codec(codec);
            free(buffer);
            return 1;
        }
    }

    unsigned char* rgb = malloc(width * height * 3);
    if (!rgb) {
        fprintf(stderr, "Error: Could not allocate memory for RGB image\n");
        opj_image_destroy(image);
        opj_stream_destroy(stream);
        opj_destroy_codec(codec);
        free(buffer);
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

    // --- Save as PNG using stb_image_write ---
    if (!stbi_write_png(output_filename.c_str(), width, height, 3, rgb, width * 3)) {
        fprintf(stderr, "Error writing PNG image\n");
    } else {
        printf("Image successfully converted: %s\n", output_filename.c_str());
    }

    // --- Cleanup ---
    free(rgb);
    opj_image_destroy(image);
    opj_stream_destroy(stream);
    opj_destroy_codec(codec);
    free(buffer);

    return 0;
}
