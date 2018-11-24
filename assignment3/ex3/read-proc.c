#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define NO_BITS      0
#define DATA0_BIT    S_IRUSR
#define DATA1_BIT    S_IWUSR
#define DATA2_BIT    S_IXUSR
#define DATA3_BIT    S_IRGRP
#define DATA4_BIT    S_IWGRP
#define DATA5_BIT    S_IXGRP
#define DATA6_BIT    S_IROTH
#define DATA7_BIT    S_IWOTH
#define HAS_DATA_BIT S_IXOTH
#define READY_BIT    S_IROTH
#define DELAY        100

mode_t data_bits[] = { DATA0_BIT, DATA1_BIT, DATA2_BIT, DATA3_BIT, DATA4_BIT, DATA5_BIT, DATA6_BIT, DATA7_BIT };

mode_t encode_char(char c) {
    mode_t bits = NO_BITS;

    for(int i = 0; i < 8; ++i) {
        if(c & (1 << i)) {
            bits |= data_bits[i];
        } else {
            bits &= ~data_bits[i];
        }
    }

    return bits;
}

char decode_char(mode_t mode) {
    char c = 0;

    for(int i = 0; i < 8; ++i) {
        if(mode & data_bits[i]) {
            c |= (1 << i);
        }
    }

    return c;
}

mode_t read_bits(const char *channel) {
    struct stat statbuf;

    if(stat(channel, &statbuf)) {
        fprintf(stderr, "stat failed (%d): %s\n", errno, strerror(errno));
        exit(1);
    }

    return statbuf.st_mode;
}

void write_bits(const char *channel, mode_t bits) {
    if(chmod(channel, bits)) {
        fprintf(stderr, "chmod failed (%d): %s\n", errno, strerror(errno));
        exit(1);
    }
}

bool check_bits(const char *channel, mode_t bits) {
    return (read_bits(channel) & bits) == bits;
}

bool check_and_get_bits(const char *channel, mode_t *out_bits, mode_t bits) {
    return ((*out_bits = read_bits(channel)) & bits) == bits;
}

void write_char(const char *channel, const char *sync, char c) {
    // 1. wait for reader to indicate it is ready to receive
    while(!check_bits(sync, READY_BIT)) {
        usleep(DELAY);
    }

    // 2. write data to channel
    write_bits(channel, encode_char(c) | HAS_DATA_BIT);

    // 3. wait till reader indicates it received the data
    while(check_bits(sync, READY_BIT)) {
        usleep(DELAY);
    }

    // 4. clear data from channel
    write_bits(channel, NO_BITS);
}

char read_char(const char *channel, const char *sync) {
    mode_t bits;

    // 1. indicate to writer that reader is ready to receive
    write_bits(sync, READY_BIT);

    // 2. wait till there is data in the channel
    while(!check_and_get_bits(channel, &bits, HAS_DATA_BIT)) {
        usleep(DELAY);
    }

    // 3. indicate to writer that reader has received the data
    write_bits(sync, NO_BITS);

    // 4. wait for writer to clear data from channel
    while(check_bits(channel, HAS_DATA_BIT)) {
        usleep(DELAY);
    }

    return decode_char(bits);
}

void write_str(const char *channel, const char *sync, const char *str) {
    while(*str) {
        write_char(channel, sync, *str++);
    }
    write_char(channel, sync, 0);
}

size_t read_str(const char *channel, const char *sync, char *str, size_t n) {
    size_t len = 0;
    char c = 1;

    while(len < n && c != 0) {
        c = str[len++] = read_char(channel, sync);
    }

    // make sure null terminator exists
    if(len == n) {
        str[len-1] = 0;
    }

    return len;
}

void init_channel(const char *channel, const char *sync) {
    write_bits(channel, NO_BITS);
    write_bits(sync, NO_BITS);
}

int main(int argc, char **argv) {
    if(argc < 5) {
        printf("usage: %s [in_channel] [in_sync] [out_channel] [out_sync]\n", argv[0]);
        puts("  in_channel:  path to input channel file (stat permission required)");
        puts("  in_sync:     path to input sync file (chmod permission required)");
        puts("  out_channel: path to output channel file (stat permission required)");
        puts("  out_sync:    path to output sync file (chmod permission required)");
        exit(1);
    }

    char buffer[256];
    const char *in_channel = argv[1];
    const char *in_sync = argv[2];
    const char *out_channel = argv[3];
    const char *out_sync = argv[4];
    init_channel(out_channel, in_sync);

    puts("receiving message...\n");
    read_str(in_channel, in_sync, buffer, sizeof(buffer));
    puts(buffer);
    puts("\nsending message...");
    write_str(out_channel, out_sync, buffer);

    return 0;
}
