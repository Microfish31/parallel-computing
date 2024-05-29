#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>
#include <lame/lame.h>
#include <time.h>

#define BUFFER_SIZE 32768

struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

// Convert double to mad_fixed_t
mad_fixed_t double_to_fixed(double value) {
    return (mad_fixed_t)(value * (1L << MAD_F_FRACBITS));
}

// Clamp function to ensure the value is within a specific range
mad_fixed_t clamp(mad_fixed_t value, mad_fixed_t min, mad_fixed_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int decode_and_encode_mp3(const char *input_file, const char *output_file, double gain) {
    FILE *input;
    FILE *output;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    struct timespec t_start, t_end, t_temp;
    double elapsedTime;

    // Open input file for reading
    input = fopen(input_file, "rb");
    if (input == NULL) {
        printf("Unable to open input file\n");
        return 1;
    }

    // Initialize libmad structures
    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    // Initialize LAME encoder
    lame_global_flags *lame = lame_init();
    lame_set_num_channels(lame, 2); // Stereo output
    lame_set_in_samplerate(lame, 48000); // Set input sample rate
    lame_set_out_samplerate(lame, 48000); // Set output sample rate
    lame_set_brate(lame, 320); // Set output bitrate (kbps)
    lame_set_quality(lame, 0); // quality=0..9.  0=best (very slow).  9=worst.
    lame_set_mode(lame, JOINT_STEREO); // Use joint stereo mode
    lame_set_VBR(lame, vbr_default); // Open VBR
    lame_init_params(lame);

    // Open output file for writing
    output = fopen(output_file, "wb");
    if (output == NULL) {
        printf("Unable to open output file\n");
        fclose(input);
        lame_close(lame);
        return 1;
    }

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

        // Start measuring time
        clock_gettime(CLOCK_REALTIME, &t_start); 

        // Decode frames
        while (1) {
            if (mad_frame_decode(&frame, &stream) != MAD_ERROR_NONE) {
                if (MAD_RECOVERABLE(stream.error)) {
                    fprintf(stderr, "Recoverable error: %s\n", mad_stream_errorstr(&stream));
                    continue; // Ignore recoverable errors
                } else {
                    fprintf(stderr, "Unrecoverable error: %s\n", mad_stream_errorstr(&stream));
                    break; // Decoding error, exit loop
                }
            }

            // Synthesize decoded audio samples
            mad_synth_frame(&synth, &frame);

            // Adjust volume
            mad_fixed_t fixed_gain = double_to_fixed(gain);
            for (unsigned int i = 0; i < synth.pcm.length; i++) {
                synth.pcm.samples[0][i] = clamp(mad_f_mul(synth.pcm.samples[0][i], fixed_gain), MAD_F_MIN, MAD_F_MAX);
                synth.pcm.samples[1][i] = clamp(mad_f_mul(synth.pcm.samples[1][i], fixed_gain), MAD_F_MIN, MAD_F_MAX);
            }

            // Encode samples to MP3 and write to output file
            short pcm_samples_left[BUFFER_SIZE];
            short pcm_samples_right[BUFFER_SIZE];
            unsigned char mp3_buffer[BUFFER_SIZE];
            for (unsigned int i = 0; i < synth.pcm.length; i++) {
                pcm_samples_left[i] = (synth.pcm.samples[0][i] >> (MAD_F_FRACBITS - 15));
                pcm_samples_right[i] = (synth.pcm.samples[1][i] >> (MAD_F_FRACBITS - 15));
            }
            int mp3_bytes = lame_encode_buffer(lame, pcm_samples_left, pcm_samples_right, synth.pcm.length, mp3_buffer, sizeof(mp3_buffer));
            if (mp3_bytes > 0) {
                fwrite(mp3_buffer, 1, mp3_bytes, output);
            }
        }
    }

    // Flush remaining MP3 data
    unsigned char mp3_buffer[BUFFER_SIZE];
    int mp3_bytes = lame_encode_flush(lame, mp3_buffer, sizeof(mp3_buffer));
    if (mp3_bytes > 0) {
        fwrite(mp3_buffer, 1, mp3_bytes, output);
    }

    // Stop measuring time
    clock_gettime(CLOCK_REALTIME, &t_end);

    // Cleanup
    lame_close(lame);
    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    // Close input and output files
    fclose(input);
    fclose(output);

    // Calculate and print elapsed time
    t_temp = diff(t_start, t_end);
    elapsedTime = t_temp.tv_sec * 1000 + (double)t_temp.tv_nsec / 1000000.0;
    printf("Parallel elapsedTime: %lf ms\n", elapsedTime);

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

    int result = decode_and_encode_mp3(input_file, output_file, gain);

    if (result == 0) {
        printf("Decoding and encoding successful. Output written to %s\n", output_file);
    } else {
        printf("Decoding and encoding failed.\n");
    }

    return result;
}
