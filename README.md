GLSL-Debugger
=============

GLSL source level debugger.

This is the Open Source public release of the project originally known
as glslDevil ( http://www.vis.uni-stuttgart.de/glsldevil/ ), by
Thomas Klein, Magnus Strengert and Thomas Ertl.

Building
------

General building avaliable using cmake build system. It automatically checks
for any dependences. Example build steps:

```
mkdir Build && cd build
cmake ..\
make
```

On windows VS project files may be used for bilding with MSVC. Cmake also support
generation of VS projects, but it may have errors in generated steps and provided
VS projects is recommended.

Windows building requires additional depencencies installed:

- flex & bison (winflexbison is recommended: http://sourceforge.net/projects/winflexbison/ 
  gnu flex and bison might work too)
- Qt library
- glut for windows ( http://user.xmission.com/~nate/glut.html 
	Instalation instructions: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/#win)
  or freeglut ( http://www.transmissionzero.co.uk/software/freeglut-devel/ )
- The Microsoft Detours ( http://research.microsoft.com/en-us/projects/detours/ ).
  The 32-bit version of Detours is available for download under a license whose
  suitability for glslDevil is somewhat indeterminate to us. The 64-bit version
  of Detours is only available by commercial license ($10,000USD), which is why no
  64-bit Windows version of glslDevil was ever produced.  

On osx build was not tested, but cmake files may be used.


Short-term goals
----------------

We need to test the build system and the source code more thoroughly. Hence, bugfixing and
testing should be a top priority for now.

Long-term goals
---------------

Replace Windows Detours dependency with apiTrace ( https://github.com/apitrace/apitrace ),
glIntercept ( https://code.google.com/p/glintercept/ ) or EasyHook ( http://easyhook.codeplex.com/ ).

Check cmake-to-VS generation and fix it if needed.

Support OSX.

Improve GLSL language grammar support to incorporate newer dialects including switch/case.

Support OpenGL contexts for newer versions of OpenGL.

Contribute
----------

We are looking for people who have an interest in this tool's capability to bring this project
back to life and move it forward, so get in touch, check out the code, try it out, fix some things
and push some changes!

Discussion is at glsl-debugger-development@googlegroups.com
https://groups.google.com/forum/?fromgroups#!forum/glsl-debugger-development
