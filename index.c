#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "hw2.h" // Assume hw2.h contains the error code definitions
#define MAX_COLOR_VALUE 255

// Structure to store RGB values
typedef struct {
    unsigned char r, g, b;
} RGB;

// Structure to store image data
typedef struct {
    int width;
    int height;
    RGB **pixels;
    RGB *color_table;
    int color_count;
} Image;
void display(Image *image)
{
    for(int i=0;i<image->height;i++)
    {
       for(int j=0;j<image->width;j++)
           // cout<<image.pixels[i][j]<<" ";
           {

           printf("%d , %d , %d  ",image->pixels[i][j].r,image->pixels[i][j].g,image->pixels[i][j].b);
           }
        
        //cout<<endl;
        printf("\n");
    }
}
// Function to allocate memory for a 2D array
RGB** allocate_2d_array(int width, int height) {
    RGB** array = (RGB**)malloc(height * sizeof(RGB*));
    if (!array) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < height; ++i) {
        array[i] = (RGB*)malloc(width * sizeof(RGB));
        if (!array[i]) {
            for (int j = 0; j < i; ++j) {
                free(array[j]);
            }
            free(array);
            fprintf(stderr, "Error: Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    return array;
}

// Function to free memory allocated for a 2D array
void free_2d_array(RGB** array, int height) {
    if (!array) return;
    for (int i = 0; i < height; ++i) {
        free(array[i]);
    }
    free(array);
}
// Function to read PPM file
Image* read_ppm(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        exit(INPUT_FILE_MISSING);
    }

    char magic[3];
    fscanf(file, "%2s", magic);

    if (strcmp(magic, "P3") != 0) {
        fprintf(stderr, "Error: Not a valid PPM file\n");
        exit(INPUT_FILE_MISSING);
    }

    Image *image = (Image*)malloc(sizeof(Image));
    if (!image) {
        fclose(file);
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &image->width, &image->height);
image->color_count=0;
image->color_table=NULL;
    int max_color;
    fscanf(file, "%d", &max_color);
    if (max_color != MAX_COLOR_VALUE) {
        fprintf(stderr, "Error: Invalid max color value in PPM file\n");
        exit(INPUT_FILE_MISSING);
    }
 image->pixels = allocate_2d_array(image->width, image->height);

    // Read pixel data
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            fscanf(file, "%hhu %hhu %hhu", &image->pixels[i][j].r, &image->pixels[i][j].g, &image->pixels[i][j].b);
        }
    }

    fclose(file);
    return image;

}

// Function to read SBU file
Image* read_sbu(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        exit(INPUT_FILE_MISSING);
    }

    char magic[4];
    fscanf(file, "%3s", magic);

    if (strcmp(magic, "SBU") != 0) {
        fprintf(stderr, "Error: Not a valid SBU file\n");
        exit(INPUT_FILE_MISSING);
    }

    Image *image = (Image*)malloc(sizeof(Image));
    if (!image) {
        fclose(file);
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &image->width, &image->height);

    int color_count;
    fscanf(file, "%d", &image->color_count);
    color_count=image->color_count;
    if (color_count <= 0) {
        fprintf(stderr, "Error: Invalid color count in SBU file\n");
        exit(INPUT_FILE_MISSING);
    }

    // Allocate memory for color table (not used in this version)
    RGB *color_table = (RGB*)malloc(color_count * sizeof(RGB));
    if (!color_table) {
        fclose(file);
        free(image);
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Read color table
    for (int i = 0; i < color_count; ++i) {
        fscanf(file, "%hhu %hhu %hhu", &color_table[i].r, &color_table[i].g, &color_table[i].b);
    }

    // Allocate memory for pixel data (2D array)
    image->pixels = allocate_2d_array(image->width, image->height);

    // Read pixel data
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            char symbol;
            int count, color_index;
            if (fscanf(file, "%c", &symbol) == EOF) {
                fprintf(stderr, "Error: Unexpected end of file\n");
                fclose(file);
              //  free_2d_array(image->pixels,image->height);
                free(image);
                free(color_table);
                exit(INPUT_FILE_MISSING);
            }
            //printf(" symbol is %c \n",symbol);
            if (symbol == '*') {
                fscanf(file, "%d %d", &count, &color_index);
  // printf(" color index and count is %d %d \n",color_index,count);;

                if (count < 2 || color_index < 0 || color_index >= color_count) {
                    fprintf(stderr, "Error: Invalid run-length encoding in SBU file\n");
                    fclose(file);
                  //  free_2d_array(image->pixels,image->height);
                    free(image);
                    free(color_table);
                    exit(INPUT_FILE_MISSING);
                }
                for (int k = 0; k < count; ++k) {
                           image->pixels[i][j].b = color_table[color_index].b;
                            image->pixels[i][j].r = color_table[color_index].r;
                            image->pixels[i][j].g = color_table[color_index].g;                    j++;
                    if (j >= image->width) {
                        j = 0;
                        i++;
                    }
                    if (i >= image->height) {
                        break;
                    }
                }
                j--; // Adjust for the loop increment
            } else {
              //  ungetc(symbol, file);
              color_index=-1;
                fscanf(file, "%d", &color_index);
                if(color_index==-1)
                {
                    j--;
                    continue;
                }
             //   printf(" color index is %d \n",color_index);
                if (color_index < 0 || color_index >= color_count) {
                    fprintf(stderr, "Error: Invalid color index in SBU file\n");
                    fclose(file);
                   // free_2d_array(image->pixels,image->height);
                    free(image);
                    free(color_table);
                    exit(INPUT_FILE_MISSING);
                }
                image->pixels[i][j].b = color_table[color_index].b;
                                image->pixels[i][j].r = color_table[color_index].r;
                image->pixels[i][j].g = color_table[color_index].g;

            }
        }
    }

    fclose(file);
    image->color_table=color_table;
    color_table=NULL;
    return image;
}

// Function to write image data to an SBU file
void write_sbu(const char* filename, Image* image) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        exit(OUTPUT_FILE_UNWRITABLE);
    }

    // Write SBU header
    fprintf(file, "SBU\n");
    fprintf(file, "%d %d\n", image->width, image->height);

    // Write color table
    fprintf(file, "%d\n", image->color_count);
    for (int i = 0; i < image->color_count; ++i) {
        fprintf(file, "%d %d %d ", image->color_table[i].r, image->color_table[i].g, image->color_table[i].b);
    }
    fprintf(file, "\n");

    // Write pixel data
    int run_length = 0;
    int prev_index = -1;
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            int current_index = -1;
            for (int k = 0; k < image->color_count; ++k) {
                if (image->pixels[i][j].r == image->color_table[k].r &&
                    image->pixels[i][j].g == image->color_table[k].g &&
                    image->pixels[i][j].b == image->color_table[k].b) {
                    current_index = k;
                    break;
                }
            }
            if (current_index == prev_index) {
                run_length++;
            } else {
                if (run_length > 1) {
                    fprintf(file, "*%d %d ", run_length, prev_index);
                } else if (run_length == 1) {
                    fprintf(file, "%d ", prev_index);
                }
                run_length = 1;
                prev_index = current_index;
            }
        }
    }

    // Write the last run length
    if (run_length > 1) {
        fprintf(file, "*%d %d ", run_length, prev_index);
    } else if (run_length == 1) {
        fprintf(file, "%d ", prev_index);
    }

    fclose(file);
}
// Function to determine file type and call appropriate function
Image* read_image(const char* filename) {
    char *ext = strrchr(filename, '.');
    if (!ext) {
        fprintf(stderr, "Error: File has no extension\n");
        exit(INPUT_FILE_MISSING);
    }
    ext++; // Move past the period character
    if (strcmp(ext, "ppm") == 0) {
        return read_ppm(filename);
    } else if (strcmp(ext, "sbu") == 0) {
        return read_sbu(filename);
    } else {
        fprintf(stderr, "Error: Unsupported file format\n");
        exit(INPUT_FILE_MISSING);
    }
}

// Function to free memory allocated for image data
// Function to free memory allocated for an Image
void free_image(Image* image) {
    if (!image) return;

    // Free memory allocated for pixel data (2D array)
    if (image->pixels!=NULL) {
        free_2d_array(image->pixels, image->height);
    }

    // Free memory allocated for color table
//     if (image->color_count!=0)
//     {

// printf("im in deleting");
//     if( image->color_table!=NULL) {
//         free(image->color_table);
//     }
//     }

    // Free memory allocated for Image structure
    free(image);
}

// Function to copy a rectangular region from an image
Image* copy_image(Image* image, int row, int col, int width, int height) {
    Image *copied_image = (Image*)malloc(sizeof(Image));
    if (!copied_image) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    copied_image->width = width;
    copied_image->height = height;
    copied_image->pixels = allocate_2d_array(width, height);

    for (int i = 0; i < height &&row+i<image->height; ++i) {
        for (int j = 0; j < width && col+j<image->width; ++j) {
            copied_image->pixels[i][j] = image->pixels[row + i][col + j];
        }
    }
  //  printf("copy image before\n");
//display(copied_image);
   // printf("copy image after\n");

    return copied_image;
}

// Function to paste a copied image onto another image at specified position
void paste_image(Image* dest_image, Image* src_image, int dest_row, int dest_col) {
    for (int i = 0; i < src_image->height && dest_row+i<dest_image->height; ++i) {
        for (int j = 0; j < src_image->width && dest_col+j<dest_image->width; ++j) {
            dest_image->pixels[dest_row + i][dest_col + j].b = src_image->pixels[i][j].b;
            dest_image->pixels[dest_row + i][dest_col + j].g = src_image->pixels[i][j].g;
            dest_image->pixels[dest_row + i][dest_col + j].r = src_image->pixels[i][j].r;

        }
    }
  //  display(src_image);
  //  display(dest_image);
}


// Function to paste a copied image onto another image at specified position
// void paste_image(Image* dest_image, Image* src_image, int dest_row, int dest_col) {
//     for (int i = 0; i < src_image->height; ++i) {
//         for (int j = 0; j < src_image->width; ++j) {
//             dest_image->pixels[(dest_row + i) * dest_image->width + (dest_col + j)] = src_image->pixels[i * src_image->width + j];
//         }
//     }
// }
// Function to write image data to PPM file
void write_ppm(const char* filename, Image* image) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Unable to open file %s for writing\n", filename);
        exit(OUTPUT_FILE_UNWRITABLE);
    }

    // Write header
    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n", image->width, image->height);
    fprintf(file, "%d\n", MAX_COLOR_VALUE);

    // Write pixel data
    for (int i = 0; i < image->height; ++i) {
        for (int j = 0; j < image->width; ++j) {
            fprintf(file, "%d %d %d ", image->pixels[i][j].r, image->pixels[i][j].g, image->pixels[i][j].b);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}
void Write_image(const char* filename,Image *image) {
    char *ext = strrchr(filename, '.');
    if (!ext) {
        fprintf(stderr, "Error: File has no extension\n");
        exit(INPUT_FILE_MISSING);
    }
    ext++; // Move past the period character
    if (strcmp(ext, "ppm") == 0) {
        write_ppm(filename,image);
    } else if (strcmp(ext, "sbu") == 0) {
        write_sbu(filename,image);
    } else {
        fprintf(stderr, "Error: Unsupported file format\n");
        exit(INPUT_FILE_MISSING);
    }
}
int main(int argc, char *argv[]) {
    // Variables to store command-line argument values
    char *input_file = NULL;
    char *output_file = NULL;
    int copy_row = -1, copy_col = -1, copy_width = -1, copy_height = -1;
    int paste_row = -1, paste_col = -1;

    // Flags to check if arguments are provided
    int i_flag = 0, o_flag = 0, c_flag = 0, p_flag = 0;

    // Parsing command-line arguments using getopt()
    int opt;
    while ((opt = getopt(argc, argv, "i:o:c:p:")) != -1) {
        switch (opt) {
            case 'i':
                if (i_flag) {
                    fprintf(stderr, "Error: Duplicate argument -i.\n");
                    exit(DUPLICATE_ARGUMENT);
                }
                i_flag = 1;
                input_file = optarg;
                break;
            case 'o':
                if (o_flag) {
                    fprintf(stderr, "Error: Duplicate argument -o.\n");
                    exit(DUPLICATE_ARGUMENT);
                }
                o_flag = 1;
                output_file = optarg;
                break;
            case 'c':
                if (c_flag) {
                    fprintf(stderr, "Error: Duplicate argument -c.\n");
                    exit(DUPLICATE_ARGUMENT);
                }
                c_flag = 1;
                // Tokenizing the argument to extract individual parameters
                if (sscanf(optarg, "%d,%d,%d,%d", &copy_row, &copy_col, &copy_height, &copy_width) != 4) {
                    fprintf(stderr, "Error: Invalid argument format for -c.\n");
                    exit(C_ARGUMENT_INVALID);
                }
                break;
            case 'p':
                if (p_flag) {
                    fprintf(stderr, "Error: Duplicate argument -p.\n");
                    exit(DUPLICATE_ARGUMENT);
                }
                p_flag = 1;
                // Tokenizing the argument to extract individual parameters
                if (sscanf(optarg, "%d,%d", &paste_row, &paste_col) != 2) {
                    fprintf(stderr, "Error: Invalid argument format for -p.\n");
                    exit(P_ARGUMENT_INVALID);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -i <input_file> -o <output_file> -c <copy_row,copy_col,copy_width,copy_height> -p <paste_row,paste_col>\n", argv[0]);
                exit(UNRECOGNIZED_ARGUMENT); // Exit with error code for unrecognized argument
        }
    }

    // Check if required arguments are provided
    if (!i_flag || !o_flag) {
        fprintf(stderr, "Error: Missing required argument(s).\n");
        exit(MISSING_ARGUMENT); // Exit with error code for missing argument
    }

    // Check if copy argument is missing when paste argument is provided
    if ((p_flag) && !(c_flag)) {
        fprintf(stderr, "Error: Copy argument is missing.\n");
        exit(C_ARGUMENT_MISSING); // Exit with error code for missing copy argument
    }

    // Placeholder for additional validations (e.g., input file exists, output file is writable)

    // If everything is fine, continue with the program
    // printf("Input file: %s\n", input_file);
    // printf("Output file: %s\n", output_file);
    // printf("Copy region: row=%d, col=%d, width=%d, height=%d\n", copy_row, copy_col, copy_width, copy_height);
    // printf("Paste position: row=%d, col=%d\n", paste_row, paste_col);
Image *image = read_image(input_file);

    // Display image properties
    // printf("Image width: %d\n", image->width);
    // printf("Image height: %d\n", image->height);
    //display(image);
  Image *copied_image = copy_image(image, copy_row, copy_col, copy_height,copy_width);

//     // Example: Paste the copied image onto (row=20, col=20) of the original image
     paste_image(image, copied_image, paste_row, paste_col);
//     // Free memory allocated for image data
//     display(image);

Write_image(output_file,image);
    free_image(image);
   // printf("iam before free");
   // display(copied_image);
//     // Placeholder for further processing
     free_image(copied_image);

    return 0;
}
