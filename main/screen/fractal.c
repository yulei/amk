/**
 * @file fractal.c
 */

#include <stdint.h>
#include <stdlib.h>
#include "timer.h"
#include "fractal.h"
#include "screen.h"

enum {
    FRACTAL_JULIA,
    FRACTAL_MANDELBROT,
};

#define ITERATION_MAX   128
#define REAL_CONSTANT   (0.285f)
#define IMG_CONSTANT    (0.01f)
#define ANIMATION_SIZE  26
const uint16_t animation_zoom[ANIMATION_SIZE] = {
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110
};

static uint16_t fractal_lut[ITERATION_MAX];

#define FRACTAL_WIDTH   64
#define FRACTAL_HEIGHT  64
static uint16_t fractal_buffer[FRACTAL_WIDTH*FRACTAL_HEIGHT];

static void fractal_lut_init(void);
void fractal_julia(uint16_t width, uint16_t height, uint16_t x, uint16_t y, uint16_t zoom, uint16_t *buffer);
void fractal_mandelbrot(float left, float top, float xside, float yside, uint16_t *buffer);

void fractal_init(void)
{
    fractal_lut_init();
    srand(timer_read32());
}

void fractal_task(void)
{
    static uint8_t index = 0;
    //fractal_julia(64,64,rand()%64,rand()%64,animation_zoom[index], fractal_buffer);
    fractal_julia(64,64,32,32,animation_zoom[index]/8, fractal_buffer);
    //fractal_mandelbrot(-1.75, -0.25, 0.45, 0.25, fractal_buffer);
    screen_fill_rect(0, 0, 64, 64, fractal_buffer, 64*64*2);
    index = (index+1) % ANIMATION_SIZE;
}

void fractal_julia(uint16_t width, uint16_t height, uint16_t x, uint16_t y, uint16_t zoom, uint16_t *buffer)
{
    for (uint16_t i_y = 0; i_y < height; i_y++) {
        for (uint16_t i_x = 0; i_x < width; i_x++) {
            float num_real = i_y - y;
            num_real = num_real / zoom;
            float num_img = i_x - x;
            num_img = num_img / zoom;
            uint8_t iterator= 0;
            float rayon = 0;
            while ((iterator < ITERATION_MAX - 1) && (rayon < 2))
            {
                float tmp1 = num_real * num_real;
                float tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                rayon = tmp1 + tmp2;
                iterator++;
            }

            /* Store the value in the buffer */
            buffer[i_x+i_y*width] = fractal_lut[iterator];
        }
    }
}

void fractal_mandelbrot(float left, float top, float xside, float yside, uint16_t *buffer) 
{ 
    float xscale, yscale, zx, zy, cx, tempx, cy; 
    int x, y; 
    int maxx, maxy, iterator; 
  
    // getting maximum value of x-axis of screen 
    maxx = 64; 
  
    // getting maximum value of y-axis of screen 
    maxy = 64;
  
    // setting up the xscale and yscale 
    xscale = xside / maxx; 
    yscale = yside / maxy; 
  
    // calling rectangle function 
    // where required image will be seen 
    //rectangle(0, 0, maxx, maxy); 
  
    // scanning every point in that rectangular area. 
    // Each point represents a Complex number (x + yi). 
    // Iterate that complex number 
    for (y = 1; y <= maxy - 1; y++) { 
        for (x = 1; x <= maxx - 1; x++) 
        { 
            // c_real 
            cx = x * xscale + left; 
  
            // c_imaginary 
            cy = y * yscale + top; 
  
            // z_real 
            zx = 0; 
  
            // z_imaginary 
            zy = 0; 
            iterator = 0; 
  
            // Calculate whether c(c_real + c_imaginary) belongs 
            // to the Mandelbrot set or not and draw a pixel 
            // at coordinates (x, y) accordingly 
            // If you reach the Maximum number of iterations 
            // and If the distance from the origin is 
            // greater than 2 exit the loop 
            while ((zx * zx + zy * zy < 4) && (iterator< ITERATION_MAX)) 
            { 
                // Calculate Mandelbrot function 
                // z = z*z + c where z is a complex number 
  
                // tempx = z_real*_real - z_imaginary*z_imaginary + c_real 
                tempx = zx * zx - zy * zy + cx; 
  
                // 2*z_real*z_imaginary + c_imaginary 
                zy = 2 * zx * zy + cy; 
  
                // Updating z_real = tempx 
                zx = tempx; 
  
                // Increment count 
                iterator ++; 
            } 
  
            // To display the created fractal 
            buffer[x+y*64] = fractal_lut[iterator];
        } 
    } 
} 

static void fractal_lut_init(void)
{
    /* Color map generation */
    for (int i = 0; i < ITERATION_MAX; i++)
    {
        /* Generate red, green and blue values */
        uint16_t red    = (i * 8 * 256 / ITERATION_MAX) % 256;
        uint16_t green  = (i * 6 * 256 / ITERATION_MAX) % 256;
        uint16_t blue   = (i * 4 * 256 / ITERATION_MAX) % 256;
        fractal_lut[i]  = ((red&0x00F8) << 8) | ((green&0x00FC) << 3) | ((blue%0x00F8) >> 3);
        //uint16_t tmp = ((red&0x00F8) << 8) | ((green&0x00FC) << 3) | ((blue%0x00F8) >> 3);
        //fractal_lut[i] = (tmp<<8) | (tmp>>8);
    }
}