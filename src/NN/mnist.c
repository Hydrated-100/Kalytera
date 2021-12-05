#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <err.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>



#include "include/mnist_file.h"
#include "include/neural_network.h"

#define STEPS 10000
#define BATCH_SIZE 100
#define PATH "../NN/save"



const char * train_images_file = "data/train-images-idx3-ubyte";
const char * train_labels_file = "data/train-labels-idx1-ubyte";
const char * test_images_file = "data/t10k-images-idx3-ubyte";
const char * test_labels_file = "data/t10k-labels-idx1-ubyte";



Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/**
 * Calculate the accuracy of the predictions of a neural network on a dataset.
 */
float calculate_accuracy(mnist_dataset_t * dataset, neural_network_t * network)
{
    float activations[MNIST_LABELS], max_activation;
    int i, j, correct, predict;

    // Loop through the dataset
    for (i = 0, correct = 0; i <(int)dataset->size; i++) {
        // Calculate the activations for each image using the neural network
        neural_network_hypothesis(&dataset->images[i], network, activations);

        // Set predict to the index of the greatest activ        neural_network_hypothesis(&img,&network,activations);
        for (j = 0, predict = 0, max_activation = activations[0]; j < MNIST_LABELS; j++) {
            if (max_activation < activations[j]) {
                max_activation = activations[j];
                predict = j;
            }
        }

        // Increment the correct count if we predicted the right label
        if (predict == dataset->labels[i]) {
            correct++;
        }
    }

    // Return the percentage we predicted correctly as the accuracy
    return ((float) correct) / ((float) dataset->size);
}

int main()
{
    FILE *file;
    file  = fopen(PATH,"r");
    int save = 1;
    if (NULL != file) 
        if (fgetc(file) == EOF)
            save = 0;
    fclose(file);

    mnist_dataset_t * train_dataset, * test_dataset;
    mnist_dataset_t batch;
    neural_network_t network;
    float loss, accuracy;
    int i, batches;

    // Read the datasets from the files
    train_dataset = mnist_get_dataset(train_images_file, train_labels_file);
    test_dataset = mnist_get_dataset(test_images_file, test_labels_file);
    if(save == 0)
    {
        // Initialise weights and biases with random values
        neural_network_random_weights(&network);

        // Calculate how many batches (so we know when to wrap around)
        batches = train_dataset->size / BATCH_SIZE;

        for (i = 0; i < STEPS; i++) {
            // Initialise a new batch
            mnist_batch(train_dataset, &batch, 100, i % batches);
            int end = 0;
            if (STEPS-i == 1)
                end ++;
            // Run one step of gradient descent and calculate the loss
            loss = neural_network_training_step(&batch, &network, 0.5,end,PATH);
            if(i%1000 == 0)
            {
                // Calculate the accuracy using the whole test datasetal_loss += neural_network_gradient_update(&dataset->images[i], network, &gradient, dataset->labels[i]);
            
            accuracy = calculate_accuracy(test_dataset, &network);
            printf("Step %04d\tAverage Loss: %.2f\tAccuracy: %.3f\n", i, loss / batch.size, accuracy);
            }
        }
        //printf("%d\n",i);
        // Cleanup
        mnist_free_dataset(train_dataset);
        mnist_free_dataset(test_dataset);

    }
    else
    {
        char grid[82];
        FILE *file;
        file  = fopen(PATH,"r");
        char * line  = NULL;
        size_t len = 0; 
        for(int i = 0;i<MNIST_LABELS;i++)
        {
            getline(&line,&len,file);
            network.b[i] = atof(line);
        }
        for(int i = 0;i<MNIST_LABELS;i++)
        {
            for(int j = 0; j<MNIST_IMAGE_SIZE;j++)
            {
                getline(&line,&len,file);
                network.W[i][j] = atof(line);
            }
        }
        fclose(file);
        float activations[10],max_activation;
        int j,predict;
        for(int i = 0; i<81;i++)
        {
            mnist_image_t img;
            char path[12];
            if(i<10)
            {
                snprintf(path,12,"number_0%d.png",i);
            }
            else
            {
                snprintf(path,12,"number_%d.png",i);
            }
            SDL_Init(SDL_INIT_VIDEO);
            SDL_Surface *imageSDL;
            imageSDL = IMG_Load(path);
            int average;
            for(int j = 0;j<28;j++)
            {
                for(int k = 0; k<28;k++)
                {
                    SDL_Color rgb;
                    uint32_t pixel = getpixel(imageSDL,k,j);
                    SDL_GetRGB(pixel,imageSDL->format,&rgb.r,&rgb.g,&rgb.b);

                    if(rgb.b == 255)
                    {
                        img.pixels[j*28+k] = 0;
                    }
                    else
                    {
                        if(k>4 && k<24 && j>4 && j<24)
                        {
                            img.pixels[j*28+k] = 255;
                            average++;
                        }
                        else
                            img.pixels[j*28+k] = 0;
                    }
                }
            }
            if(average == 0)
                grid[i] = 0;
            else
            {
                average = 0;
                neural_network_hypothesis(&img,&network,activations);
                for (j = 0, predict = 0, max_activation = activations[0]; j < MNIST_LABELS; j++)
                {
                    if (max_activation < activations[j]) {
                        max_activation = activations[j];
                        predict = j;
                    }
                }
                grid[i] = predict;
            }
            FILE *fp;
            fp = fopen("../Sudoku/grid","w");
            for(int k = 0;k<9;k++)
            {
                for(int j = 0;j<9;j++)
                {
                    if (j == 3 || j == 6)
                        fprintf(fp,"%c",' ');
                    fprintf(fp,"%d",grid[k*9+j]);
                    if (j == 8 && k != 8)
                        fprintf(fp,"%s","\n");
                }
                if(k == 2 || k == 5)
                    fprintf(fp,"%c",'\n');
            }
            fprintf(fp,"%c",'\n');
            fclose(fp);
        }
    }
    return 0;
}

