/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id3_reader.h"
#include "error_handling.h"
#include "type.h"

static int version;
/**
TODO: Add documention as sample given
 */
unsigned int swap(unsigned int x)
{
    void *ptr = &x;
    for (int i = 0; i < sizeof(unsigned int) / 2; i++)
    {
        char temp = *((char *)ptr + i);
        *((char *)ptr + i) = *((char *)ptr + 4 - i - 1);
        *((char *)ptr + 4 - i - 1) = temp;
    }
    return x;
}

static char *read_comm_frame(FILE *fp) {
    unsigned int frame_size;
    fread(&frame_size, 4, 1, fp);
    frame_size = swap(frame_size);

    // read flags (2 bytes)
    fseek(fp, 2, SEEK_CUR);

    // read encoding (1 byte)
    unsigned char encoding;
    fread(&encoding, 1, 1, fp);

    // read language (3 bytes)
    char lang[4];
    fread(lang, 1, 3, fp);
    lang[3] = '\0';

    // read description (null-terminated string)
    char desc_buf[256];
    int i = 0;
    char c;
    while (i < sizeof(desc_buf)-1 && fread(&c, 1, 1, fp) == 1) {
        if (c == '\0') break;
        desc_buf[i++] = c;
    }
    desc_buf[i] = '\0';

    // remaining is the actual comment text
    int remaining = frame_size - (1 + 3 + (i+1));
    char *comment = malloc(remaining + 1);
    if (!comment) return NULL;

    fread(comment, 1, remaining, fp);
    comment[remaining] = '\0';

    return comment;
}


Status checkfilename(const char *filename)
{
    if (strstr(filename, ".mp3") == NULL)
    {
        display_error("Unsupported File Format\n");
        return failure;
    }
    return success;
}

char *read_text_frame(FILE *fp)
{
    unsigned int frame_size;
    fread(&frame_size, 4, 1, fp);
    frame_size = swap(frame_size);

    fseek(fp, 3, SEEK_CUR); //skip 2 bytes of frame flags and 1 byte of encoding byte
    char *content = malloc(frame_size);
    if (!content)
        return NULL;

    fread(content, frame_size - 1, 1, fp);
    content[frame_size - 1] = '\0';
    return content;
}

TagData *read_id3_tags(const char *filename)
{
    // Implementation for reading ID3 tags
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        display_error("File opening error\n");
        return NULL;
    }

    unsigned char header[10];
    if (fread(header, 1, 10, fp) < 10)
    {
        display_error("Error reading ID3 header\n");
        fclose(fp);
        return NULL;
    }

    // Check "ID3" identifier
    if (header[0] != 'I' || header[1] != 'D' || header[2] != '3')
    {
        display_error("Not a valid ID3 tag\n");
        fclose(fp);
        return NULL;
    }

    int version_major = header[3];
    int version_minor = header[4];
    if (version_major != 3 || version_minor != 0)
    {
        version = version_major;
        printf("Error: Unsupported ID3v2.%d version (only v2.3 supported)\n",version);
        fclose(fp);
        return NULL;
    }

    version = 3;
    // Allocate struct to store data
    TagData *data = create_tag_data();
    if (!data) {
        display_error("Error in Memory Allocation for struct\n");
        fclose(fp);
        return NULL;
    }

    char tag[5];
    while (fread(tag, 4, 1, fp) == 1) {
        tag[4] = '\0';
        if (tag[0] == '\0') break;  // padding reached

        if (strcmp(tag, "TIT2") == 0) {
            data->title = read_text_frame(fp);
        }
        else if (strcmp(tag, "TRCK") == 0) {
            data->track = read_text_frame(fp);
        }
        else if (strcmp(tag, "TPE1") == 0) {
            data->artist = read_text_frame(fp);
        }
        else if (strcmp(tag, "TALB") == 0) {
            data->album = read_text_frame(fp);
        }
        else if (strcmp(tag, "TYER") == 0) {
            data->year = read_text_frame(fp);
        }
        else if (strcmp(tag, "TCON") == 0) {
            data->genre = read_text_frame(fp);
        }
        else if (strcmp(tag, "COMM") == 0) {
            data->comment = read_comm_frame(fp);
        }
        else {
            // skipping unknown or unhandled frame
            unsigned int frame_size;
            fread(&frame_size, 4, 1, fp);
            frame_size = swap(frame_size);

            fseek(fp, frame_size + 2, SEEK_CUR); 
        }
    }
    fclose(fp);
    return data;
}

void display_metadata(const TagData *data)
{
    // Implementation for displaying metadata
    printf("-------------------------\n");
    printf("MP3 TAG READER & EDITOR : \n");
    printf("-------------------------\n");
    printf("ID3 Version : v2.%d\n",version);
    printf("-------------------------\n");
    if (data->title)  printf("Title   : %s\n", data->title);
    if (data->track)  printf("Track   : %s\n", data->track);
    if (data->artist) printf("Artist  : %s\n", data->artist);
    if (data->album)  printf("Album   : %s\n", data->album);
    if (data->year)   printf("Year    : %s\n", data->year);
    if (data->genre)  printf("Genre   : %s\n", data->genre);
    if (data->comment)printf("Comment : %s\n", data->comment);
    printf("-------------------------\n\n");
}


void view_tags(const char *filename)
{
    if (checkfilename(filename) == failure)
    {
        return;
    }
    TagData *data = read_id3_tags(filename);
    if (data == NULL)
    {
        display_error("Failed to read ID3 tags.\n");
        return;
    }
    display_metadata(data);
    free_tag_data(data);
}
