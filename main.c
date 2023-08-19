#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include <mpi.h>
#include <omp.h>
#include "header.h"

double absDouble(double n);
int isMatch(image_object img, image_object obj, int i, int j, double threshold);
int* objectsToIntArr(image_object* objects, int num_of_objs);
image_object* intArrToObjects(int *arr);

int main(){
    double start_time = omp_get_wtime(), end_time;
    // Init all the data we need using the input file
    // printf("Start\n");
    Data data = init_data();
    // printf("After init data\n");

    // int * res = objectsToIntArr(data.objects, data.objects_length);
    // int num_of_objs = res[1];
    // image_object * objects = intArrToObjects(res);

    // for(int i = 0 ; i < num_of_objs ; i++){
    //     print_img(objects[i]);
    // }

    // Init an array of matching points struct to save all points we've found
    matching_point *** imagesMatchPoints = (matching_point ***)malloc(sizeof (matching_point**) * data.images_length);
    for(int i = 0 ; i < data.images_length; i++){
        imagesMatchPoints[i] = NULL;
    }

    // printf("num of imgs %d\n", data.images_length);
    // printf("num of objs %d\n", data.objects_length);

    // For loop through all images
    for(int i = 0 ; i < data.images_length; i++){
        int objectsFoundCounter = 0;
        matching_point **points = (matching_point **)malloc(sizeof(matching_point*) * OBJ_TO_FIND);
        
        // For loop through all objects
        for(int j = 0 ; j < data.objects_length; j++){
            image_object img = data.images[i];
            image_object obj = data.objects[j];

            // Limit is define to be the last cell we want to check for a match
            int limit = img.dim - obj.dim + 1;

            // Searching the object inside the image if found return the point, else NULL 
            matching_point *point = searchObjectInImage(img, obj, limit, data.threshold);
            if(point != NULL){
                points[objectsFoundCounter++] = point;
                if(objectsFoundCounter == OBJ_TO_FIND)
                    break;
            }
        }

        // If 3 objects found we save them
        if(objectsFoundCounter == OBJ_TO_FIND){
            imagesMatchPoints[i] = points;
        }
    }

    // Loop through all what we've found and print
    for (int i = 0; i < data.images_length; i++) {
        if(imagesMatchPoints[i] != NULL){
            printf("Picture %d: found Objects:", data.images[i].id);
            for (int j = 0; j < OBJ_TO_FIND; j++) {
                printf(" %d Position(%d,%d) ;", imagesMatchPoints[i][j]->id, imagesMatchPoints[i][j]->i, imagesMatchPoints[i][j]->j);
            }
            printf("\n");
        } else{
            printf("Picture %d: No three different Objects were found\n", data.images[i].id);
        }
    }

    end_time = omp_get_wtime();
    printf("Elapsed time = %f seconds\n", end_time - start_time);

    return 0;
}

matching_point *searchObjectInImage(image_object img, image_object obj, int limit, double threshold) {
    
    if(limit <= 0)
        return NULL;
    // printf("Searching in img %d in obj %d\n", img.id, obj.id);
    matching_point* point = NULL;
    int n;
    #pragma omp parallel for
    for( n = 0 ; n < limit * limit ; n++){
        int i = n / limit;
        int j = n % limit;
        if(isMatch(img, obj, i, j, threshold) == 1){
            point = (matching_point *)malloc(sizeof (matching_point));
            point->id = obj.id;
            point->i = i;
            point->j = j;
        }
    }

    return point;
}

int isMatch(image_object img, image_object obj, int startI, int startJ, double threshold) {

    double sum = 0;
    for(int i = 0 ; i < obj.dim; i++){
        for(int j = 0 ; j < obj.dim; j++){
            double p = img.matrix[(startI + i) * img.dim + (startJ + j)];
            double o = obj.matrix[i*obj.dim + j];
            sum += absDouble((p-o)/p);
        }
    }
    if(sum/(obj.dim * obj.dim) < threshold)
        return 1;
    return 0;
}

double absDouble(double n){
    
    if (n > 0)
        return n;
    return n * -1;
}

int* objectsToIntArr(image_object* objects, int num_of_objs){
    int length = 2, i, j, *res, counter = 0;
    
    for( i = 0 ; i < num_of_objs ; i++){
        // for id and dim
        length += 2;
        //for the matrix
        length += objects[i].dim * objects[i].dim;
    }

    res = malloc(sizeof(int) * length);
    res[counter++] = length;
    res[counter++] = num_of_objs;
    for( i = 0 ; i < num_of_objs ; i++){
        res[counter++] = objects[i].id;
        res[counter++] = objects[i].dim;
        for(j = 0 ; j < objects[i].dim * objects[i].dim ; j++){
            res[counter++] = objects[i].matrix[j];
        }
    }

    return res;
}

image_object* intArrToObjects(int *arr){
    int counter = 0, length, num_of_objs, i, j;
    image_object* objects;

    length = arr[counter++];
    num_of_objs = arr[counter++];

    objects = malloc(sizeof(image_object) * num_of_objs);

    for( i = 0 ; i < num_of_objs ; i++){
        objects[i].id = arr[counter++];
        objects[i].dim = arr[counter++];
        objects[i].matrix = malloc(sizeof(int) * (objects[i].dim * objects[i].dim));
        for(j = 0 ; j < objects[i].dim * objects[i].dim ; j++){
            objects[i].matrix[j] = arr[counter++];
        }
    }
    return objects;
}