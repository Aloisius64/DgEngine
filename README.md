# DgEngine
A simple C++ game engine.

The game engine is based on OpenGL core profile (v4.5), and has been developed for passion in order to learn computer graphics.
In its implementation have been implemented several techniques, including:
  - Lights (Directional, Point and Spot lights),
  - Shadows (For all tipe of lights),
  - SSAO (Screen space ambient occlusion),
  - Occlusion rendering,
  - Frustum culling,
  - HDR,
  - Bloom,
  - OIT (order-independet-transparency),
  - Volume rendering,
  - Skinning,
  - Skybox.
  
The game engine provides support for physics through Nvidia PhysX (v3.3.2), which was integrated into the core.

Also you can harness the power of the device "Oculus Rift" to view VR scenes.

Another feature of the game engine is the ability to leverage the OpenCAL library, with which you can view all the data concerning cellular automata based models are specified in a scene.
 
# Requirements
Currently the project works only on Microsoft Windows systems, so Microsoft Visual Studio is the IDE recommended.
All libraries used are included in the project folders.
It required the use of the Visual Studio compiler 2012, while you can use the Visual Studio version you want.
To run the code you simply open the Visual Studio project and compile.

Do not upgrade the compiler used by the project and make sure the compiler that you are using is Visual Studio version 2012.
