cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(Task_4-I2CSensor)

target_sources(app PRIVATE sensor.c)
