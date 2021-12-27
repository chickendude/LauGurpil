#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_CHARACTERS_IN_LINE 1024
#define MAX_CHECKPOINTS 100

typedef struct Checkpoint
{
    int x, y;
    int speed;
    int is_vertical;
} Checkpoint;

Checkpoint checkpoints[MAX_CHECKPOINTS];
int num_checkpoints = 0;

// Function definitions

/** Reads the TMX file into the [checkpoints] array */
int read_tmx(char *filename);

void write_file(char *filename, char *filename_upper, char *output_dir);

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

    char* output_dir;
    if (argc > 2) {
        output_dir = argv[2];
    } else {
        output_dir = malloc(sizeof("."));
        output_dir = ".";
    }

    if (read_tmx(argv[1])) write_file(filename, filename_upper, output_dir);
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

void write_file(char *filename, char *filename_upper, char *output_dir)
{
    // Create .h file

    char output_filename[32];
    sprintf(output_filename, "%s/%s.h", output_dir, filename);

    if (output_dir[0] != '.' && stat(output_dir, NULL) == -1) {
        mkdir(output_dir, 0700);
    }
    FILE *file = fopen(output_filename, "w");

    fprintf(file, "#define %s_CHECKPOINT_COUNT %d\n\n", filename_upper,
            num_checkpoints);

    // TODO: Put this line in .h file
    fprintf(file,
            "extern const Checkpoint %s_checkpoint_markers[%s_CHECKPOINT_COUNT];\n",
            filename, filename_upper);
    fclose(file);

    // Create .c file

    sprintf(output_filename, "%s/%s.c", output_dir, filename);

    file = fopen(output_filename, "w");

    fprintf(file, "const Checkpoint %s_checkpoint_markers[] = {\n", filename);

    for (int i = 0; i < num_checkpoints; i++)
    {
        int x = checkpoints[i].x;
        int y = checkpoints[i].y;
        int speed = checkpoints[i].speed;
        char *is_vertical = checkpoints[i].is_vertical ? "VERT" : "HORZ";

        fprintf(file, "\t{%d, %d, %d, %s},\n", x, y, speed, is_vertical);
    }
    fseek(file, -2, SEEK_END);
    fprintf(file, "\n};");
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
        char *speed_str = strstr(line, "speed");
        if (speed_str != NULL)
        {
            int speed = extract_number(speed_str);
            checkpoints[num_checkpoints].speed = speed;
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
    while (*number_str < '0' || *number_str > '9') number_str++;

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
