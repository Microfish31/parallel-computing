#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <mad.h>
#include <lame/lame.h>

#define NUM_THREADS 16

typedef struct {
    unsigned char *data;
    size_t length;
    size_t start;
    size_t end;
    float gain;
    short *output_buffer;
    size_t output_offset;
    size_t output_size;
} ThreadData;

void adjust_volume(short *samples, size_t count, float gain) {
    for (size_t i = 0; i < count; ++i) {
        int new_sample = (int)(samples[i] * gain);
        if (new_sample > 32767) {
            new_sample = 32767;
        } else if (new_sample < -32768) {
            new_sample = -32768;
        }
        samples[i] = (short)new_sample;
    }
}

void* process_block(void *arg) {
    ThreadData *threadData = (ThreadData*) arg;
    // printf("Thread processing data block from byte offset %zu to %zu\n", threadData->start, threadData->end);
    // printf("Output buffer offset: %zu\n", threadData->output_offset);

    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;

    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    stream.buffer = threadData->data + threadData->start;
    stream.bufend = threadData->data + threadData->end;
    // printf("stream.buffer: %p\n", threadData->data + threadData->start);
    // printf("stream.bufend: %p\n", threadData->data + threadData->end);

    size_t output_pos = threadData->output_offset;

    mad_stream_buffer(&stream, threadData->data, threadData->length);

    while (stream.buffer < stream.bufend) {
        if (mad_frame_decode(&frame, &stream) == 0) {
            mad_synth_frame(&synth, &frame);
            for (int i = 0; i < synth.pcm.length; i++) {

                if (output_pos >= threadData->output_size) {
                    fprintf(stderr, "Output buffer overflow\n");
                    mad_synth_finish(&synth);
                    mad_frame_finish(&frame);
                    mad_stream_finish(&stream);
                    return NULL;
                }
                short left_sample = synth.pcm.samples[0][i] >> (MAD_F_FRACBITS - 15);
                short right_sample = synth.pcm.samples[1][i] >> (MAD_F_FRACBITS - 15);
                adjust_volume(&left_sample, 1, threadData->gain);
                adjust_volume(&right_sample, 1, threadData->gain);
                threadData->output_buffer[output_pos++] = left_sample;
                threadData->output_buffer[output_pos++] = right_sample;
            }
        } else {
            if (MAD_RECOVERABLE(stream.error)) {
                continue;
            } else {
                fprintf(stderr, "Unrecoverable frame error at byte offset %lu (error code: %s)\n",
                        (unsigned long) (stream.this_frame - threadData->data),
                        mad_stream_errorstr(&stream));
                break;
            }
        }
        mad_stream_sync(&stream);
    }

    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    return NULL;
}


void increase_volume(const char* input_file, const char* output_file, float gain) {
    FILE *fin;
    unsigned char *data;
    size_t file_size;
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    fin = fopen(input_file, "rb");
    if (fin == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    fseek(fin, 0, SEEK_END);
    file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    data = (unsigned char*)malloc(file_size);
    if (data == NULL) {
        perror("Error allocating memory");
        fclose(fin);
        exit(1);
    }

    fread(data, 1, file_size, fin);
    fclose(fin);

    // Estimation: Each byte of MP3 roughly corresponds to 4 bytes of PCM (stereo, 16-bit samples)
    size_t output_size = file_size * 4; 
    short *output_buffer = (short*)malloc(output_size * sizeof(short));
    if (output_buffer == NULL) {
        perror("Error allocating output buffer");
        free(data);
        exit(1);
    }

    size_t block_size = file_size / NUM_THREADS;
    if (file_size % NUM_THREADS != 0) {
        block_size++;
    }

    printf("File size: %zu bytes\n", file_size);
    printf("Number of threads: %d\n", NUM_THREADS);
    printf("Block size per thread: %zu bytes\n", block_size);

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].data = data;
        thread_data[i].length = file_size;
        thread_data[i].start = i * block_size;
        thread_data[i].end = (i == NUM_THREADS - 1) ? file_size : (i + 1) * block_size;
        thread_data[i].gain = gain;
        thread_data[i].output_buffer = output_buffer;
        thread_data[i].output_offset = thread_data[i].start;
        thread_data[i].output_size = output_size;
        pthread_create(&threads[i], NULL, process_block, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Initialize LAME encoder
    lame_t lame = lame_init();
    if (lame == NULL) {
        fprintf(stderr, "Failed to initialize LAME encoder\n");
        free(data);
        free(output_buffer);
        exit(1);
    }

    lame_set_in_samplerate(lame, 44100);
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    FILE *fout = fopen(output_file, "wb");
    if (fout == NULL) {
        perror("Error opening output file");
        free(data);
        lame_close(lame);
        free(output_buffer);
        exit(1);
    }

    unsigned char mp3_buffer[8192];
    int mp3_bytes;

    mp3_bytes = lame_encode_buffer_interleaved(lame, output_buffer, output_size / 2, mp3_buffer, sizeof(mp3_buffer));
    if (mp3_bytes < 0) {
        fprintf(stderr, "LAME encoding error: %d\n", mp3_bytes);
    } else {
        fwrite(mp3_buffer, 1, mp3_bytes, fout);
    }

    mp3_bytes = lame_encode_flush(lame, mp3_buffer, sizeof(mp3_buffer));
    if (mp3_bytes < 0) {
        fprintf(stderr, "LAME flushing error: %d\n", mp3_bytes);
    } else {
        fwrite(mp3_buffer, 1, mp3_bytes, fout);
    }

    fclose(fout);
    lame_close(lame);
    free(data);
    free(output_buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input file> <output file> <gain>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];
    float gain = atof(argv[3]);

    increase_volume(input_file, output_file, gain);

    printf("Volume increased by a factor of %.2f and saved to %s\n", gain, output_file);

    return 0;
}
