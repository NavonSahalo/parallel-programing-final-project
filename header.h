

//
// Created by Navon Sahalu 312188626
//
#define FILE_NAME "input_demo.txt"
#define OBJ_TO_FIND 3

typedef struct image_object{
    int id;
    int dim;
    int * matrix;
} image_object;

typedef struct Data{
    double threshold;
    image_object * images;
    int images_length;
    image_object * objects;
    int objects_length;
} Data;

typedef struct matching_point{
    int id;
    int i;
    int j;
} matching_point;


Data init_data();
void print_img(image_object img);
int isMatch(image_object img, image_object obj, int i, int j, double threshold);
matching_point *searchObjectInImage(image_object img, image_object obj, int limit, double threshold);