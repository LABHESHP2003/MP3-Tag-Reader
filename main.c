/**
 * @file main.c
 * @brief Main entry point for the MP3 Tag Reader application.
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "id3_reader.h"
#include "id3_writer.h"
#include "error_handling.h"

/**
 * @brief Displays the help message for the MP3 Tag Reader application.
 */
void display_help()
{
    printf("\n------------ MP3 TAG READER - HELP ------------\n");
    printf("Usage: mp3tagreader [options] filename\n");
    printf("Options:\n");
    printf("  --help                  Display help\n");
    printf("  -v                      View tags\n");
    printf("  -e <arguments> <value>  Edit tags\n");
    printf("Arguments:\n");
    printf(" -t     Modifies a Title tag\n");
    printf(" -T     Modifies a Track tag\n");
    printf(" -a     Modifies an Artist tag\n");
    printf(" -A     Modifies an Album tag\n");
    printf(" -y     Modifies a Year tag\n");
    printf(" -C     Modifies a Comment tag\n");
    printf(" -g     Modifies a Genre tag\n");
    printf("-----------------------------------------------\n\n");
}

/**
 * @brief Main function to handle command-line arguments and execute appropriate actions.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        display_help();
        return failure;
    }

    if (strcmp(argv[1], "--help") == success)
    {
        display_help();
        return success;
    }
    else if (strcmp(argv[1], "-v") == success)
    {
        if (argc < 3)
        {
            display_error("Missing filename\n");
            return failure;
        }
        view_tags(argv[2]);
    }
    else if (strcmp(argv[1], "-e") == success)
    {
        if (argc < 5)
        {
            display_error("Usage of Edit: mp3tagreader -e <arg> <value> filename\n");
            display_error("Display help : mp3tagreader --help\n");
            return failure;
        }
        const char *tag = argv[2];
        const char *value = argv[3];
        const char *filename = argv[4];

        Status status;
        if (strcmp(tag, "-t") == 0)
            status = edit_tag(filename, "TIT2", value);
        else if (strcmp(tag, "-T") == 0)
            status = edit_tag(filename, "TRCK", value);
        else if (strcmp(tag, "-a") == 0)
            status = edit_tag(filename, "TPE1", value);
        else if (strcmp(tag, "-A") == 0)
            status = edit_tag(filename, "TALB", value);
        else if (strcmp(tag, "-y") == 0)
            status = edit_tag(filename, "TYER", value);
        else if (strcmp(tag, "-C") == 0)
            status = edit_tag(filename, "COMM", value);
        else if (strcmp(tag, "-g") == 0)
            status = edit_tag(filename, "TCON", value);
        else
        {
            display_error("Unknown tag option\n");
            return failure;
        }

        if (status == success)
        {
            printf("Tag updated successfully!\n");
            return success;
        }
        else
        {
            display_error("Failed to update tag.\n");
            return failure;
        }
    }
    else
    {
        display_error("Unknown option\n");
        display_error("Display help : mp3tagreader --help\n");

        //display_help();
        return failure;
    }

    return success;
}
