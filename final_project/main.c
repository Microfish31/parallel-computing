#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>
#include <lame/lame.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 32768
#define NUM_THREADS 3 // Number of threads for parallel encoding

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

struct ThreadData {
    struct mad_synth *synth;
    lame_global_flags *lame;
    FILE *input;
    double gain;
    off_t start_offset; // Start offset for this thread
    off_t end_offset;   // End offset for this thread
};

// Thread function for encoding
void *encode_thread(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    struct mad_frame frame;
    struct mad_stream stream;
    mad_frame_init(&frame);
    mad_stream_init(&stream);
    short pcm_samples_left[BUFFER_SIZE];
    short pcm_samples_right[BUFFER_SIZE];
    unsigned char mp3_buffer[BUFFER_SIZE];

    // Move file pointer to the start of the segment
    fseek(data->input, data->start_offset, SEEK_SET);

    // Open output file
    char output_filename[50];
    snprintf(output_filename, sizeof(output_filename), "output_%lu.mp3", pthread_self());
    FILE *output = fopen(output_filename, "wb");
    if (output == NULL) {
        printf("Unable to open output file\n");
        return NULL;
    }

    // Process frames until reaching the end of the segment
    while (ftell(data->input) < data->end_offset) {
        // Read and decode frames
        if (mad_frame_decode(&frame, &stream) != MAD_ERROR_NONE) {
            // Error handling
            if (MAD_RECOVERABLE(stream.error)) {
                fprintf(stderr, "Recoverable error: %s\n", mad_stream_errorstr(&stream));
                continue; // Ignore recoverable errors
            } else {
                fprintf(stderr, "Unrecoverable error: %s\n", mad_stream_errorstr(&stream));
                break; // Decoding error, exit loop
            }
        }

        // Synthesize decoded audio samples
        mad_synth_frame(data->synth, &frame);

        // Adjust volume
        mad_fixed_t fixed_gain = double_to_fixed(data->gain);
        for (unsigned int i = 0; i < data->synth->pcm.length; i++) {
            data->synth->pcm.samples[0][i] = clamp(mad_f_mul(data->synth->pcm.samples[0][i], fixed_gain), MAD_F_MIN, MAD_F_MAX);
            data->synth->pcm.samples[1][i] = clamp(mad_f_mul(data->synth->pcm.samples[1][i], fixed_gain), MAD_F_MIN, MAD_F_MAX);
        }

        // Encode samples to MP3
        for (unsigned int i = 0; i < data->synth->pcm.length; i++) {
            pcm_samples_left[i] = (data->synth->pcm.samples[0][i] >> (MAD_F_FRACBITS - 15));
            pcm_samples_right[i] = (data->synth->pcm.samples[1][i] >> (MAD_F_FRACBITS - 15));
        }
        int mp3_bytes = lame_encode_buffer(data->lame, pcm_samples_left, pcm_samples_right, data->synth->pcm.length, mp3_buffer, sizeof(mp3_buffer));

        // Write to output file
        if (mp3_bytes > 0) {
            fwrite(mp3_buffer, 1, mp3_bytes, output);
        }
    }

    // Close output file
    fclose(output);

    mad_frame_finish(&frame);
    mad_stream_finish(&stream);
    return NULL;
}


int decode_and_encode_mp3(const char *input_file, double gain) {
    FILE *input;
    struct mad_synth synth;
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];
    off_t file_size;

    // Open input file for reading
    input = fopen(input_file, "rb");
    if (input == NULL) {
        printf("Unable to open input file\n");
        return 1;
    }

    // Get file size
    fseek(input, 0, SEEK_END);
    file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    // Initialize libmad structures
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

    // Calculate segment size for each thread
    off_t segment_size = file_size / NUM_THREADS;

    // Create threads for parallel encoding
    for (int i = 0; i < NUM_THREADS; i++) {
        // Calculate start and end offsets for each thread
        off_t start_offset = i * segment_size;
        off_t end_offset = (i == NUM_THREADS - 1) ? file_size : start_offset + segment_size;

        // Assign thread data
        thread_data[i].synth = &synth;
        thread_data[i].lame = lame;
        thread_data[i].input = input;
        thread_data[i].gain = gain;
        thread_data[i].start_offset = start_offset;
        thread_data[i].end_offset = end_offset;

        // Create thread
        pthread_create(&threads[i], NULL, encode_thread, &thread_data[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    lame_close(lame);
    mad_synth_finish(&synth);

    // Close input file
    fclose(input);

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

