cmake_minimum_required(VERSION 3.0.0)

project(bat5110 C CXX)

file(GLOB bat5110_file CONFIGURE_DEPENDS "*.h" "*.cpp")

add_executable(bat5110 ${bat5110_file})

target_link_arduino_libraries(bat5110 PRIVATE "Adafruit BusIO" "Adafruit GFX Library" AUTO_PRIVATE)
target_link_arduino_libraries("Adafruit GFX Library" PUBLIC "Adafruit BusIO")
target_link_arduino_libraries("Adafruit PCD8544 Nokia 5110 LCD library" PUBLIC "Adafruit GFX Library")

target_enable_arduino_upload(bat5110)

