/**
 * @file id3_writer.c
 * @brief Implementation of functions for writing and editing ID3 tags in MP3 files.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "id3_writer.h"
#include "id3_reader.h"
#include "id3_utils.h"


/**
 * @brief Write a modified COMM frame into the new file (forces eng + empty description).
 */
static void write_comm_frame(FILE *org, FILE *new, const char *comment) {
    unsigned int old_size_be;
    fread(&old_size_be, 4, 1, org);   // read old frame size
    unsigned int old_size = swap(old_size_be);

    // new size = encoding(1) + lang(3) + desc(1) + text
    unsigned int new_size = 1 + 3 + 1 + strlen(comment);
    unsigned int new_size_be = swap(new_size);

    // write new size
    fwrite(&new_size_be, 4, 1, new);

    // copy flags (2 bytes)
    unsigned char flags[2];
    fread(flags, 1, 2, org);
    fwrite(flags, 1, 2, new);

    // payload
    unsigned char encoding = 0x00;  // ISO-8859-1
    fwrite(&encoding, 1, 1, new);

    fwrite("eng", 1, 3, new);       // language
    unsigned char null_desc = 0x00; // empty description
    fwrite(&null_desc, 1, 1, new);

    fwrite(comment, 1, strlen(comment), new);

    // skip old payload in original file
    fseek(org, old_size, SEEK_CUR);
}

/**
 * @brief Write a modified text frame into the new file, reusing the original encoding byte.
 */
static void write_text_frame(FILE *org, FILE *new, const char *data) {
    unsigned int old_size_be;
    fread(&old_size_be, 4, 1, org);   // read old frame size
    unsigned int old_size = swap(old_size_be);

    // calculate new size (text length + encoding byte)
    unsigned int new_size = strlen(data) + 1;
    unsigned int new_size_be = swap(new_size);

    // write new size
    fwrite(&new_size_be, 4, 1, new);

// copy flags (2 bytes)
    unsigned char flags[2];
    fread(flags, 1, 2, org);
    fwrite(flags, 1, 2, new);

    // force ISO-8859-1 encoding
    unsigned char encoding = 0x00;
    fwrite(&encoding, 1, 1, new);

    // now write the actual text
    fwrite(data, 1, strlen(data), new);

    // skip old payload in original (we consumed encoding)
    fseek(org, old_size, SEEK_CUR);
}

/**
 * @brief Copy an unmodified frame as-is from org → new.
 */
static void copy_frame(FILE *org, FILE *new) {
    unsigned int size_be;
    fread(&size_be, 4, 1, org);
    fwrite(&size_be, 4, 1, new);

    unsigned int size = swap(size_be);

    unsigned char flags_enc[3];
    fread(flags_enc, 1, 3, org);
    fwrite(flags_enc, 1, 3, new);

    // copy frame content safely
    char *buffer = malloc(size);
    if (!buffer) {
        display_error("Memory allocation failed\n");
        fseek(org, size, SEEK_CUR); // skip if allocation fails
        return;
    }
    fread(buffer, 1, size, org);
    fwrite(buffer, 1, size, new);
    free(buffer);
}

/**
 * @brief Update the ID3 header size in the temp file.
 */
static void update_header_size(FILE *new, long tag_size) {
    // tag_size excludes the 10-byte header
    unsigned int be_size = swap(tag_size);
    fseek(new, 6, SEEK_SET); // size field starts at byte 6
    fwrite(&be_size, 4, 1, new);
    fseek(new, 0, SEEK_END); // restore file position
}

Status write_id3_tags(const char *filename, const TagData *data) {
    FILE *org = fopen(filename, "rb");
    if (!org) {
        display_error("Error opening original file\n");
        return failure;
    }

    FILE *new = fopen("temp.mp3", "wb+");
    if (!new) {
        display_error("Error creating temp file\n");
        fclose(org);
        return failure;
    }

    // Copy 10-byte header
    char header[10];
    fread(header, 1, 10, org);
    fwrite(header, 1, 10, new);

    char frame_id[5];
    while (fread(frame_id, 1, 4, org) == 4) {
        frame_id[4] = '\0';
        if (frame_id[0] == '\0') break; // reached padding

        fwrite(frame_id, 1, 4, new);

        if (strcmp(frame_id, "TIT2") == 0 && data->title) {
            write_text_frame(org, new, data->title);
        } else if (strcmp(frame_id, "TRCK") == 0 && data->track) {
            write_text_frame(org, new, data->track);
        } else if (strcmp(frame_id, "TPE1") == 0 && data->artist) {
            write_text_frame(org, new, data->artist);
        } else if (strcmp(frame_id, "TALB") == 0 && data->album) {
            write_text_frame(org, new, data->album);
        } else if (strcmp(frame_id, "TYER") == 0 && data->year) {
            write_text_frame(org, new, data->year);
        } else if (strcmp(frame_id, "TCON") == 0 && data->genre) {
            write_text_frame(org, new, data->genre);
        } else if (strcmp(frame_id, "COMM") == 0 && data->comment) {
            write_comm_frame(org, new, data->comment);
        } else {
            copy_frame(org, new);
        }
    }

    // Copy the rest of the file (audio)
    int c;
    while ((c = fgetc(org)) != EOF) {
        fputc(c, new);
    }

    // Compute tag size = everything written between header and audio
    long end_pos = ftell(new);
    long tag_size = end_pos - 10; // exclude header
    update_header_size(new, tag_size);

    fclose(org);
    fclose(new);

    // Replace old file
    remove(filename);
    rename("temp.mp3", filename);

    return success;
}

int edit_tag(const char *filename, const char *tag, const char *value) {
    TagData *data = read_id3_tags(filename);
    if (!data) {
        display_error("Failed to read ID3 tags.\n");
        return failure;
    }

    if (strcmp(tag, "TIT2") == 0) {
        free(data->title);
        data->title = strdup(value);
    } else if (strcmp(tag, "TRCK") == 0) {
        free(data->track);
        data->track = strdup(value);
    } else if (strcmp(tag, "TPE1") == 0) {
        free(data->artist);
        data->artist = strdup(value);
    } else if (strcmp(tag, "TALB") == 0) {
        free(data->album);
        data->album = strdup(value);
    } else if (strcmp(tag, "TYER") == 0) {
        free(data->year);
        data->year = strdup(value);
    } else if (strcmp(tag, "TCON") == 0) {
        free(data->genre);
        data->genre = strdup(value);
    } else if (strcmp(tag, "COMM") == 0) {
        free(data->comment);
        data->comment = strdup(value);
    } else {
        display_error("Unknown tag identifier\n");
        free_tag_data(data);
        return failure;
    }

    Status result = write_id3_tags(filename, data);
    free_tag_data(data);
    return result;
}
