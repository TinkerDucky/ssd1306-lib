mkdir -p ./build
g++ example_rpi.cpp -o ./build/example_rpi -lbcm2835 -I "../../src" -Wall