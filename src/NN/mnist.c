#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <err.h>



#include "include/mnist_file.h"
#include "include/neural_network.h"

#define STEPS 10000
#define BATCH_SIZE 100
#define PATH "../NN/save"



const char * train_images_file = "data/train-images-idx3-ubyte";
const char * train_labels_file = "data/train-labels-idx1-ubyte";
const char * test_images_file = "data/t10k-images-idx3-ubyte";
const char * test_labels_file = "data/t10k-labels-idx1-ubyte";

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
            }
            accuracy = calculate_accuracy(test_dataset, &network);
            printf("Step %04d\tAverage Loss: %.2f\tAccuracy: %.3f\n", i, loss / batch.size, accuracy);
        }
        //printf("%d\n",i);
        // Cleanup
        mnist_free_dataset(train_dataset);
        mnist_free_dataset(test_dataset);

    }
    else
    {
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
        mnist_image_t img = test_dataset->images[0];
        neural_network_hypothesis(&img,&network,activations);
        for (j = 0, predict = 0, max_activation = activations[0]; j < MNIST_LABELS; j++) {
            if (max_activation < activations[j]) {
                max_activation = activations[j];
                predict = j;
            }
        }

        printf("%d,%d",predict,test_dataset->labels[0]);
    }

    return 0;
}
