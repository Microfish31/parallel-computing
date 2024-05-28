#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>

#define BUFFER_SIZE 8192

// Convert double to mad_fixed_t
mad_fixed_t double_to_fixed(double value) {
    return (mad_fixed_t)(value * (1L << MAD_F_FRACBITS));
}

int decode_mp3(const char *input_file, const char *output_file, double gain) {
    FILE *input;
    FILE *output;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;

    // Open input file for reading
    input = fopen(input_file, "rb");
    if (input == NULL) {
        printf("Unable to open input file\n");
        return 1;
    }

    // Open output file for writing
    output = fopen(output_file, "wb");
    if (output == NULL) {
        printf("Unable to open output file\n");
        fclose(input);
        return 1;
    }

    // Initialize libmad structures
    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    // Set libmad input stream options
    mad_stream_buffer(&stream, NULL, 0); // Clear input stream buffer
    mad_stream_options(&stream, MAD_OPTION_IGNORECRC); // Ignore CRC errors

    // Read and decode the input file
    while (1) {
        // Read data from input file into input stream buffer
        unsigned char buffer[BUFFER_SIZE];
        int bytes_read = fread(buffer, 1, sizeof(buffer), input);
        if (bytes_read == 0) {
            break; // End of file reached
        }

        // Add data to libmad input stream
        mad_stream_buffer(&stream, buffer, bytes_read);

        // Decode frames
        while (1) {
            if (mad_frame_decode(&frame, &stream) != MAD_ERROR_NONE) {
                if (MAD_RECOVERABLE(stream.error)) {
                    continue; // Ignore recoverable errors
                } else {
                    break; // Decoding error, exit loop
                }
            }

            // Synthesize decoded audio samples
            mad_synth_frame(&synth, &frame);

            // Adjust volume
            mad_fixed_t fixed_gain = double_to_fixed(gain);
            for (unsigned int i = 0; i < synth.pcm.length; i++) {
                synth.pcm.samples[0][i] = mad_f_mul(synth.pcm.samples[0][i], fixed_gain);
                synth.pcm.samples[1][i] = mad_f_mul(synth.pcm.samples[1][i], fixed_gain);
            }

            // Write audio samples to output file
            fwrite(synth.pcm.samples[0], sizeof(mad_fixed_t), synth.pcm.length, output);
            fwrite(synth.pcm.samples[1], sizeof(mad_fixed_t), synth.pcm.length, output);
        }
    }

    // Cleanup libmad structures
    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    // Close input and output files
    fclose(input);
    fclose(output);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input file> <output file> <gain>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];
    double gain = atof(argv[3]);

    int result = decode_mp3(input_file, output_file, gain);

    if (result == 0) {
        printf("Decoding successful. Output written to %s\n", output_file);
    } else {
        printf("Decoding failed.\n");
    }

    return result;
}
