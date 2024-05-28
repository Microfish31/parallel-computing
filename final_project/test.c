#include <stdio.h>
#include <mad.h>

int main() {
    FILE *file;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    mad_timer_t timer;

    // Open the file to be decoded
    file = fopen("input.mp3", "rb");
    if (file == NULL) {
        printf("Unable to open file\n");
        return 1;
    }

    // Initialize libmad structures
    mad_stream_init(&stream);
    mad_frame_init(&frame);
    mad_synth_init(&synth);

    // Set libmad input stream
    mad_stream_buffer(&stream, NULL, 0); // Clear input stream buffer
    mad_stream_options(&stream, MAD_OPTION_IGNORECRC); // Ignore CRC errors

    // Read and decode the file
    while (1) {
        // Read file data into input stream buffer
        unsigned char buffer[8192];
        int bytesRead = fread(buffer, 1, sizeof(buffer), file);
        if (bytesRead == 0) {
            break; // End of file reached
        }

        // Add data to libmad input stream
        mad_stream_buffer(&stream, buffer, bytesRead);

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

            // Write audio samples to output device or file
            mad_timer_add(&timer, frame.header.duration);
        }
    }

    // Cleanup libmad structures
    mad_synth_finish(&synth);
    mad_frame_finish(&frame);
    mad_stream_finish(&stream);

    // Close file
    fclose(file);

    return 0;
}
