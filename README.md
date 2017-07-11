# Pattern Tracking Controller

Library for real-time arrow recognition, using [OpenCV](http://opencv.org/) to access a computer-connected camera.
This library uses a custom event class, calling them according to the arrow orientation. This could be used to control a player in a simple 2D game.

<p align="center">
  <img src ="https://user-images.githubusercontent.com/8783766/28068965-f5cfa7a6-6647-11e7-88b3-d1d564c1db92.gif" />
</p>

## Goals

This library has been made as a project for our Image Processing course. We *do not use* **OpenCV** for arrow detection. The goal was to learn something and not to simply make calls to a powerfull library. Consequently, the code may have some overheads, and the comparison we made between our processing time and **OpenCV** ones, is clearly showing us that our code could be optimized a lot.

## Integration

The library uses [OpenCV](http://opencv.org/) to access the camera. We also use [CMake](https://cmake.org/) to easily build the library.
You can build it using:
```sh
mkdir build && cd build && cmake .. && make ptc
```

## Samples

Do no forget to download the [SFML 2](https://www.sfml-dev.org/) dependency, to compile the game sample.

You can run the sample buy using the given shell script:
```sh
./run.sh <target>
```

Where **target** can be replaced by **invaders** to check the space-invader like game, or **test** to check the real time detection.

Alternatively, you can run the samples with:
```sh
mkdir build && cd build && cmake .. && make samples
```
or you can choose the sample you want:
```sh
mkdir build && cd build && cmake .. && make <sample>
```

## How it currently works

#### OpenCV to use the video device

In order to use the video device, we use the *VideoCapture* object exposed by OpenCV. The project being now finished, it could be a good idea to totally remove OpenCV from our pipeline.

#### Image Pretreatment

We apply few treatments to the image before beginning the recognition:
* Image is turned to grayscale. The template is only black and white, we do not need 3 components for the detection.
* Image is blured using a [Gaussian blur](https://en.wikipedia.org/wiki/Gaussian_blur) with a 5*5 kernel to reduce artifacts and avoid detecting wrong contours at next step.
* Image is scaled down using a basic [bilinear interpolation](https://en.wikipedia.org/wiki/Bilinear_interpolation) to reduce complexity for next steps.
* Image is binarized using a Gaussian adaptive threshold.

#### Contour extraction

We apply a variant of the [Chain Code algorithm](https://en.wikipedia.org/wiki/Chain_code), describe [here](http://users.utcluj.ro/~rdanescu/PI-L6e.pdf). The algorithm is a bit slow, but acceptable. It could be a good idea to replace this algorithm with Suzuki 85, which seems to be really faster.

#### Contour simplification

Trying to match an arrow from a contour composed of hundred of pixels is hard. To solve this, we implemented the [Ramer–Douglas–Peucker algorithm](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm), reducing the number of points in the contour.

#### Arrow matching

The last step is to use the simplified set of points to say whether the contour represents the template arrow, or not. For this, the algorithm tries to match the angles in the contour in their order of apparition. If the contour is detected as matching the template, we then extract the orientation of the head.

## TODO

* [ ] Remove OpenCV dependency
* [ ] Implement SURF algorithm
* [ ] Optimize findContour method
