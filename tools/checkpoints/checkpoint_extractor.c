#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHARACTERS_IN_LINE 1024
#define MAX_CHECKPOINTS 100

typedef struct Progress
{
    int x, y;
    int is_vertical;
} Progress;

Progress checkpoints[MAX_CHECKPOINTS];
int num_checkpoints = 0;


// Function definitions

/** Reads the TMX file into the [checkpoints] array */
int read_tmx(char *filename);

void write_file(char *filename, char *filename_upper);

int find_checkpoints(FILE *file);

void extract_checkpoints(FILE *file);

void extract_filename(const char *filename, char *output, char *output_upper);

int extract_number(char *number_str);

// Function declarations

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please include the file you want to convert:\n");
        printf("$ checkpoint_extractor track.tmx\n\n");
        return 0;
    }

    char filename[32];
    char filename_upper[32];
    extract_filename(argv[1], filename, filename_upper);

    if (read_tmx(argv[1])) write_file(filename, filename_upper);
    return 0;
}

int read_tmx(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Unable to locate file");
        fclose(file);
        return 0;
    }

    if (!find_checkpoints(file))
    {
        printf("Checkpoints could not be found in the file.");
        fclose(file);
        return 0;
    }

    extract_checkpoints(file);
    fclose(file);
    return 1;
}

void write_file(char *filename, char *filename_upper)
{
    FILE *file = fopen("output.c", "w");

    // TODO: Don't use hardcoded name
    fprintf(file, "#define %s_CHECKPOINT_COUNT %d\n\n", filename_upper,
            num_checkpoints);

    // TODO: Put this line in .h file
    fprintf(file,
            "extern const Checkpoint %s_checkpoint_markers[%s_CHECKPOINT_COUNT];\n",
            filename, filename_upper);
    fprintf(file, "const Checkpoint %s_checkpoint_markers[] = {\n", filename);

    for (int i = 0; i < num_checkpoints; i++)
    {
        int x = checkpoints[i].x;
        int y = checkpoints[i].y;
        char *is_vertical = checkpoints[i].is_vertical ? "VERT" : "HORZ";

        fprintf(file, "\t{%d, %d, 0, %s},\n", x, y, is_vertical);
    }
    fprintf(file, "};");
}

/**
 * Finds the start of the AI checkpoints in the .tmx file.
 *
 * @param file The .tmx file to search
 * @return 1 if found, 0 if not found
 */
int find_checkpoints(FILE *file)
{
    char line[MAX_CHARACTERS_IN_LINE];
    while (fgets(line, MAX_CHARACTERS_IN_LINE, file))
    {
        if (strstr(line, "\"checkpoints\"") != NULL) return 1;
    }
    return 0;
}

void extract_checkpoints(FILE *file)
{
    char line[MAX_CHARACTERS_IN_LINE];
    while (fgets(line, MAX_CHARACTERS_IN_LINE, file))
    {
        // Check if we've reached end of checkpoint section
        if (strstr(line, "</objectgroup>") != NULL) return;

        // Find start of x and y strings
        char *x_str = strstr(line, "x=\"");
        char *y_str = strstr(line, "y=\"");

        if (x_str != NULL && y_str != NULL)
        {
            int x = extract_number(x_str);
            int y = extract_number(y_str);
            checkpoints[num_checkpoints].x = x;
            checkpoints[num_checkpoints].y = y;
        }

        // Extract is_vertical value
        char *is_vertical_str = strstr(line, "vertical");
        if (is_vertical_str != NULL)
        {
            char *ch = strstr(line, "true");
            checkpoints[num_checkpoints].is_vertical = ch != NULL;
            num_checkpoints++;
        }
    }
}

void extract_filename(const char *filename, char *output, char *output_upper)
{
    // Only use filename, remove path if there is one
    char *char_index = strrchr(filename, '/');
    if (char_index != NULL) filename = char_index + 1;

    char_index = strrchr(filename, '.');

    int i = 0;
    while (filename + i < char_index)
    {
        output[i] = filename[i];
        output_upper[i] = (char) toupper(filename[i]);
        i++;
    }
}

int extract_number(char *number_str)
{
    number_str += 3;

    char *ch = strchr(number_str, '"');
    if (ch != NULL)
    {
        long index = ch - number_str;
        number_str[index] = '\0';
    }

    ch = strchr(number_str, '.');
    if (ch != NULL)
    {
        long index = ch - number_str;
        number_str[index] = '\0';
    }

    return atoi(number_str);
}
