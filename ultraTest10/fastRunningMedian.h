/**
This is the fastest median finder program. It is O(n):
http://forum.arduino.cc/index.php?topic=53081.msg1160999#msg1160999
It is a header-only library.

The following is a less efficient O(n^2) library:
https://playground.arduino.cc/Main/RunningMedian
https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
**/

// FILE: FastRunningMedian.h
// http://forum.arduino.cc/index.php?topic=53081.75 (use latest: there are older versions on other pages)
// This is a header-only library. Just include it in your code & use.

// AUTHOR: rkaul
// PURPOSE: RunningMedian library for Arduino
// VERSION: 0.1.01
// HISTORY:
// 0.1.00 rkaul initial version
// 0.1.01 rob.tillaart -> insertion sort
// 0.1.02 xdeschain -> insertion sort fixed bug, binary search, getHighest, getLowest, getAverage
// Released to the public domain
//
// Remarks:
// This is a lean but fast version.
// Initially, the buffer is filled with a "default_value". To get real median values
// you have to fill the object with N values, where N is the size of the sliding window.
// For example: for(int16_t i=0; i < 32; i++) myMedian.addValue(readSensor());
//
// Constructor:
// FastRunningMedian<datatype_of_content, size_of_sliding_window, default_value>
// maximim size_of_sliding_window is 255
// Methods:
// addValue(val) adds a new value to the buffers (and kicks the oldest)
// getMedian() returns the current median value
//
//
// Usage:
// #include "FastRunningMedian.h"
// FastRunningMedian<unsigned int,32, 0> myMedian;
// ....
// myMedian.addValue(value); // adds a value
// m = myMedian.getMedian(); // retieves the median
//

#include "Arduino.h"

#ifndef FastRunningMedian_h
#define FastRunningMedian_h

#include <inttypes.h>

template <typename T, int16_t N, T default_value> class FastRunningMedian {

public:
 FastRunningMedian() {
 _buffer_ptr = N;
 _window_size = N;
 _median_ptr = N / 2;

 // Init buffers
 int16_t i = _window_size;
 while (i > 0) {
 i--;
 _inbuffer[i] = default_value;
 _sortbuffer[i] = default_value;
 }
 };

 T getMedian() {
 // buffers are always sorted.
 return _sortbuffer[_median_ptr];
 }

 T getHighest() {
 // buffers are always sorted.
 return _sortbuffer[_window_size - 1];
 }

 T getLowest() {
 // buffers are always sorted.
 return _sortbuffer[0];
 }

 float getAverage() {
 float sum = 0;

 for (int16_t i = 0; i < _window_size; i++) {
 sum += _sortbuffer[i];
 }
 float value = sum / _window_size;
 return value;
 };


 float getAverage(int16_t nMedians) {
 if (nMedians > 0)
 {
 int16_t start = ((_window_size - nMedians) / 2);
 int16_t stop = start + nMedians;
 float sum = 0;
 for (int16_t i = start; i < stop; i++) {
 sum += _sortbuffer[i];
 }
 float value = sum / nMedians;
 return value;
 }
 return 0.0f;
 }

 void addValue(T new_value) {
 // comparision with 0 is fast, so we decrement _buffer_ptr
 if (_buffer_ptr == 0)
 _buffer_ptr = _window_size;

 _buffer_ptr--;

 T old_value = _inbuffer[_buffer_ptr]; // retrieve the old value to be replaced
 if (new_value == old_value)      // if the value is unchanged, do nothing
 return;

 _inbuffer[_buffer_ptr] = new_value;  // fill the new value in the cyclic buffer

 // search the old_value in the sorted buffer with binary search
 int16_t _start = 0, _end = _window_size - 1;
 int16_t _mid = 0; //middle
 while (_start <= _end)
 {
 _mid = _end + (_start - _end) / 2;    // average value...
 if (old_value == _sortbuffer[_mid]) { // If the middle element is the value, mid is the index of the found value
 break;
 }
 else if (old_value < _sortbuffer[_mid]) { // If the value is lesser than the  middle element, then the element must be in the left most region
 _end = _mid - 1;                      // pick the left half
 }
 else {                  // If the value is greater than the  middle element, then the element must be in the right most region
 _start = _mid + 1;  // pick the right half
 }
 }

 //Serial.print("found "); Serial.print(old_value); Serial.print("at location"); Serial.println(_mid);

 if (_start > _end) {
 Serial.print("ERROR old_value not found: "); Serial.println(old_value);
 }

 int16_t i = _mid;

 // insert in _sortbuffer
 if (new_value > old_value)
 {
 int16_t j = i + 1;
 while (j < _window_size && new_value > _sortbuffer[j])
 {
 _sortbuffer[j - 1] = _sortbuffer[j];
 j++;
 }
 _sortbuffer[j - 1] = new_value;
 }
 else
 {
 int16_t j = i - 1;
 while (j > -1 && new_value < _sortbuffer[j])
 {
 _sortbuffer[j + 1] = _sortbuffer[j];
 j--;
 }
 _sortbuffer[j + 1] = new_value;
 }
 }


private:
 // Pointer to the last added element in _inbuffer
 int16_t _buffer_ptr;
 // sliding window size
 int16_t _window_size;
 // position of the median value in _sortbuffer
 int16_t _median_ptr;

 // cyclic buffer for incoming values
 T _inbuffer[N];
 // sorted buffer
 T _sortbuffer[N];
};
#endif
