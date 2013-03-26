GLSL-Debugger
=============

GLSL source level debugger.

This is the Open Source public release of the project originally known as glslDevil ( http://www.vis.uni-stuttgart.de/glsldevil/ ), by Thomas Klein, Magnus Strengert and Thomas Ertl.


As it exists now, on Windows it requires the Microsoft Detours ( http://research.microsoft.com/en-us/projects/detours/ ). The 32-bit version of Detours is available for download under a license whose suitability for glslDevil is somewhat indeterminate to us. The 64-bit version of Detours is only available by commercial license ($10,000USD), which is why no 64-bit Windows version of glslDevil was ever produced.

On Linux, no additional tools should be required.

Hitherto, no OSX version of glslDevil has been produced.


Long-term goals include:

Replace Windows Detours dependency with apiTrace ( https://github.com/apitrace/apitrace ), glIntercept ( https://code.google.com/p/glintercept/ ) or EasyHook ( http://easyhook.codeplex.com/ ).

Support OSX.

Improve GLSL language grammar support to incorporate newer dialects including switch/case.

Support OpenGL contexts for newer versions of OpenGL.

We are looking for people who have an interest in this tool's capability to bring this project back to life and move it forward, so get in touch, check out the code, try it out, fix some things and push some changes!
