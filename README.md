# VB-like-C-window-wrapper-class

A simple header only C++ window wrapper that functions similar to classic VB forms

With this project, I wanted to make it simple to test out graphical and console type problems at the same time in c without getting bogged down in boiler plate windows handling code.

The idea is to capture the simplicity and immediacy of classic VB forms in a header only c++ implementation.  This is Windows only, since VB was a Windows only software, and relied a lot on Windows GDI.

I wanted to make improvements over VB forms, including faster pixel access

The VBWindow class event processing can be used either in a game loop, or with event callbacks, whichever is more appropriate.
