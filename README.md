# tinyzxing
`tinyzing` is an open-source library to read QR-codes with embedded devices in mind. This is a **simplified** version of [ZXing C++ Port](https://github.com/zxing/zxing/tree/00f634024ceeee591f54e6984ea7dd666fab22ae/cpp) adapted to use C++17 features.

## Features
- Compile-time defined parameters such as image size and maximum QR version;
- No dynamic allocations;
- No exceptions;

## Supported Formats
- QR Code up to version N (defined at compilation time)

## Dependencies
OpenCV demo depends on OpenCV >= 4.0.0.

## Configuring and Building
1. Edit `core/src/zxing/ZXingConfig.h` to define `ImageWidth` and `ImageHeight` (this must be the exact dimension of the input image) and `MaxGridSize` (this is used to compute the maximum supported QR version:
```
MaxSupportedQRVersion = (MaxGridSize - 17) / 4;
```
2. If necessary, edit the parameters marked as TODO - these were defined in a way that works for my case but can be probably computed from QR version.
3. In an empty build directory, invoke `cmake ..` to build the library and examples.

## OpenCV demo
Run `opencv_zxing` to detect QR codes using a webcam. Position the code inside the green box and observe the resulting black matrix:
![opencv_demo](https://github.com/camposm/tinyzxing/blob/200ecd7fdef6e786807df9616dcb41fd1b7459d8/docs/opencv_demo.png)

## ESP32 demo
This is a preview of the ESP32 demo. For more details and actual usage check my other repo [OvenCamera](https://github.com/camposm/ovencamera).

![esp32_demo2](https://github.com/camposm/tinyzxing/blob/200ecd7fdef6e786807df9616dcb41fd1b7459d8/docs/esp32_demo2.png)

