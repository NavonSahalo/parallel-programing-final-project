//
// Created by Navon Sahalu 312188626
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

void print_img(image_object img){
    printf("id: %d\n", img.id);
    printf("dim: %d\n", img.dim);
    int dim = img.dim;
    for(int i = 0 ; i < dim; i++){
        for(int j = 0 ; j < dim; j++){
            printf("%d\t", img.matrix[i*dim + j]);
        }
        printf("\n");
    }
}

void read_input(FILE* file, image_object *res, int length) {

    // Init variables
    char *buf = NULL;
    size_t buf_size;

    // Read 2D arrays from the file
    for (int i = 0; i < length; i++) {
        // Init variables
        image_object image;

        // Read id
        getline(&buf, &buf_size, file);
        image.id = (int)strtol(buf, NULL, 10);

        // Read dimension of image/object
        getline(&buf, &buf_size, file);
        image.dim = (int)strtol(buf, NULL, 10);

        // Allocate image
        image.matrix = (int*)malloc(sizeof (int) * image.dim * image.dim);

        // Read the image/object
        // image.matrix = malloc(sizeof(int*) * image.dim);
        for (int j = 0; j < image.dim; j++){
            getline(&buf, &buf_size, file);
            char * token = strtok(buf, " ");
            image.matrix[j*image.dim] = (int)strtol(token, NULL, 10);
            for (int k = 1; k < image.dim; k++){
                token = strtok(NULL, " ");
                image.matrix[j*image.dim + k] = (int)strtol(token, NULL, 10);
            }
        }
        // Add to list of image/object
        res[i] = image;
    }

}

Data init_data() {

    // Init variables
    int num_of_imgs, num_of_objs;
    double threshold;
    char *buf = NULL;
    size_t buf_size;
    ssize_t line_size;
    Data data;
    image_object *images, *objects;

    // Open the file for reading
    FILE* file = fopen(FILE_NAME, "r"); 

    // Read the threshold
    line_size = getline(&buf, &buf_size, file);
    threshold = strtod(buf, NULL);

    // Read images
    line_size = getline(&buf, &buf_size, file);
    num_of_imgs = (int)strtol(buf, NULL, 10);
    images = (image_object*)malloc(sizeof(image_object) * num_of_imgs);
    read_input(file, images, num_of_imgs);

    // Read objects
    line_size = getline(&buf, &buf_size, file);
    num_of_objs = (int)strtol(buf, NULL, 10);
    objects = (image_object*)malloc(sizeof(image_object) * num_of_objs);
    read_input(file, objects, num_of_objs);

    // Close the file
    fclose(file);

    // Result data
    data.threshold = threshold;
    data.images = images;
    data.images_length = num_of_imgs;
    data.objects = objects;
    data.objects_length = num_of_objs;

    return data;
}
