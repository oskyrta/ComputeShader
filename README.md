# ComputeShader
#### This repo contains my thesis project on real-time physics computing using CPU and GPU.
<img src="https://media.giphy.com/media/Zr2SKO2WbXLdhBgndl/giphy.gif" width="480" height="362"  />

It's divided into two projects. 
The first one is named `Engine`. It's a static library that contains wrappers for GLSL and OpenGL, 2D math and some helpful classes. 
The second is named `PhysicsCompute` and contains the implementation of physics computing algorithms for the CPU and GPU.

The main idea of this project is to compare the results of algorithms from different types of processors.
CPU part is written in C++, and GPU part uses OpenGl Compute Shaders.

Axis-aligned Bounding Boxes were used to search for possible collisions in the broad phase.
The Minkowski Difference and the Separating Axis Theorem were used in a narrow phase. 
Objects' velocities after collisions were calculated using impulses.
