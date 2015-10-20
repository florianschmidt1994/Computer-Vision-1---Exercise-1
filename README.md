# Task
In a given colour Image, find “superpixel” regions:
* calculate the magnitude of the colour gradient (x+y direction) and put them (together with pixel number) in a priority queue
* :1 pop lowest gradient pixel
* 4-connect the current pixel to all neighbours with
colour distance lower a given Δ (changeable by slider) - remove all connected pixels from the priority queue
* if priority queue not empty go to :1
* colour each region with its mean colour
* output the resulting image
* discuss problems and possible refinements of this algorithm

Hint:
Search `opencv\samples\cpp\*.cpp` containing keywords you need (gradient, region, mask ...)
See also core module tutorials