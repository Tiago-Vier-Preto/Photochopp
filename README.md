# Photochopp

Photochopp is a simple image editing application built using C++ and the Qt framework. It allows users to perform basic image processing tasks such as flipping images, converting to grayscale, and performing grayscale quantization. The application displays the original image alongside a processed version, with all transformations being applied to the processed image.

## Features

- Load and display images in multiple formats
- Flip images horizontally and vertically
- Convert images to grayscale
- Grayscale quantization (reduce number of shades of gray)
- Zoom in and out on images
- Reset the image to its original state
- Save the processed image in various formats
- Copy and paste images from the clipboard

## Installation

1. Clone this repository:
    ```bash
    git clone https://github.com/yourusername/photochopp.git
    cd photochopp
    ```

2. Build the project:
    - Make sure you have the Qt development environment set up.
    - Open the project in Qt Creator or use `qmake` and `make` from the command line:
      ```bash
      qmake
      make
      ```

## Usage

- **Open an Image**: Click `File` > `Open` to load an image.
- **Save the Image**: Click `File` > `Save As` to save the processed image.
- **Flip Image**: Use the `Edit` menu to flip the image horizontally or vertically.
- **Convert to Grayscale**: Click `Edit` > `Convert to Grayscale`.
- **Quantize Grayscale**: Reduce the number of shades of gray in the image by clicking `Edit` > `Grayscale Quantization` and entering the desired number of levels.
- **Zoom**: Use the `View` menu to zoom in, zoom out.

## About

Photochopp was developed by [Augusto Mattei Grohmann](https://github.com/augustomattei) and [Tiago Vier Preto](https://github.com/tiagovpreto).

