# Pattern Tracking Controller

Library for real-time arrow recognition, using [OpenCV](http://opencv.org/) to access a computer-connected camera.
This library uses a custom event class, calling them according to the arrow orientation. This could be used to control a player in a simple 2D game.

## Goals

This library has been made as a project for our Image Processing courses. We do not use **OpenCV** for arrow detection. The goal was to learn something and not to simply make calls to a powerfull library. Consequently, the code may have some overheads, and the comparison we made between our processing time and **OpenCV** ones, is clearly showing us that our code could be optimized a lot.

## What we tested

### Otsu Binarization + Custom find contours + Custom curve approximation

First of all, **Otsu** binarization was not good when the camera was submitted to overexposure. Moreover, our `findContours()` method is clearly overkill, because it was not working in real time when the area to find the contour of was half the image.

### Adaptive Gaussian Thresholding + Custom find contours + Custom curve approximation

This is the pipeline we use by default. Adaptive Gaussian Thresholding gives good edges at good speed (gives something close to Canny), and allows to find coutours 

### Surf

## Integration

The library uses [OpenCV](http://opencv.org/) to access the camera. We also use [CMake](https://cmake.org/) to easily build the library.
You can build it using:
```sh
mkdir build && cd build && cmake .. && make ptc
```

If you want to test the samples, you will have to install [SFML 2](https://www.sfml-dev.org/), used to create our sample games.

## Samples

Do no forget to download the [SFML 2](https://www.sfml-dev.org/) dependency. You can run the samples with:
```sh
mkdir build && cd build && cmake .. && make samples
```
or you can choose the sample you want:
```sh
mkdir build && cd build && cmake .. && make invaders-sample
```
