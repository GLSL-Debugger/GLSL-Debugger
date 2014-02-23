GLSL-Debugger
=============

GLSL source level debugger.

This is the Open Source public release of the project originally known
as glslDevil ( http://www.vis.uni-stuttgart.de/glsldevil/ ), by
Thomas Klein, Magnus Strengert and Thomas Ertl.


Building
------

The only available build system is cmake. Example build steps:

```
mkdir Build && cd Build
cmake ..\
make
```

Cmake may generate files for big number of make utilities, please check the cmake
documentation for further information.

It is recommended to do out-of-source build.

Windows building requires additional dependencies installed:

- flex & bison (winflexbison is recommended: http://sourceforge.net/projects/winflexbison/
  gnu flex and bison might work too)
- Qt library
- glut for windows ( not tested, http://user.xmission.com/~nate/glut.html
	Installation instructions: http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/#win)
  or freeglut ( tested, http://www.transmissionzero.co.uk/software/freeglut-devel/ )
- mhook required for windows build. It is added as submodule, use
  `git submodule update --init --recursive` if it was not checked with the tree.


On osx build was not tested, but cmake files may be used.


Short-term goals
----------------

We need to test the build system and the source code more thoroughly. Hence, bugfixing and
testing should be a top priority for now.


Long-term goals
---------------

- [x] Replace Windows Detours dependency with ~apiTrace, glIntercept or EasyHook~. Replaced with mhook.
- [x] Check cmake-to-VS generation and fix it if needed.
- [ ] Support OSX.
- [ ] Improve GLSL language grammar support to incorporate newer dialects including switch/case. (In progress)
- [ ] Support OpenGL contexts for newer versions of OpenGL.


Contribute
----------

We are looking for people who have an interest in this tool's capability to bring this project
back to life and move it forward, so get in touch, check out the code, try it out, fix some things
and push some changes!

Discussion is at glsl-debugger-development@googlegroups.com
https://groups.google.com/forum/?fromgroups#!forum/glsl-debugger-development

IRC channel at freenode: #glsl-debugger

