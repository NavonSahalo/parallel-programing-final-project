#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "header.h"

double absDouble(double n);
int isMatch(image_object img, image_object obj, int i, int j, double threshold);
int* objectsToIntArr(image_object* objects, int num_of_objs);
image_object* intArrToObjects(int *arr);

int main(int argc, char * argv[]){
    double start_time = omp_get_wtime(), end_time;
    int rank, numprocs, i, length, *arrOfObjects, num_of_objs, *procImgTable, imagesSent = 0, nkilled = 0, pointsArr[9];
    MPI_Status status;
    MPI_Init( & argc, & argv);
    MPI_Comm_size(MPI_COMM_WORLD, & numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, & rank);
    
    if(rank == 0){
        // Init all the data we need using the input file
        Data data = init_data();
        arrOfObjects = objectsToIntArr(data.objects, data.objects_length);
        length = arrOfObjects[0];
        procImgTable = malloc(sizeof(int) * numprocs);

        matching_point *** imagesMatchPoints = (matching_point ***)malloc(sizeof (matching_point**) * data.images_length);
        for(int i = 0 ; i < data.images_length; i++){
            imagesMatchPoints[i] = NULL;
        }

        // Sent all objects to all processes
        for(i = 1 ; i < numprocs ; i++)
            MPI_Send(arrOfObjects, length, MPI_INT, i, 0, MPI_COMM_WORLD);

        //Sends the first tasks
        for (i = 1; i < numprocs && imagesSent < data.images_length; i++){
            procImgTable[i] = data.images[i].id;
            imageSize = (data.images[i].dim * data.images[i].dim);

            MPI_Send( data.images[i].matrix , imageSize, MPI_INT, i , 0, MPI_COMM_WORLD);
            imagesSent++;
        }

        //Sends the remaining tasks whenever a proc is available
        while (nkilled < numprocs - 1) {
            MPI_Recv( pointsArr, 9, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, & status);
            if(pointsArr[0] != -1){
                
            }
        }
    }
    else{
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &length);

        arrOfObjects = malloc(sizeof(int) * length);

        MPI_Recv(arrOfObjects, length, MPI_INT, 0, 0, MPI_COMM_WORLD, & status);

        num_of_objs = arrOfObjects[1];
        image_object* objects = intArrToObjects(arrOfObjects);

        for(i = 0 ; i < num_of_objs ; i++){
            print_img(objects[i]);
        }
    }
    MPI_Finalize();
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