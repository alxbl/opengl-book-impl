# The OpenGL Book: Exercises Repository

This is a repository that I created while following along [the OpenGL book](http://openglbook.com)
I found it to be a great tutorial to understand the basic concepts and OpenGL
API, and would strongly recommend anyone with an interest in 3D programming to
take a look at the book.

The book's license is CC-BY SA. My code is public domain if you'd
like to rip it apart. It follows very closely from the examples in the book,
asides from my own naming/syntax.

If you're looking for the book's source, it is [open sourced](https://github.com/openglbook).


## Building

`cd build` and type `cmake ../ && make`.

Chapters 1 through 3 should run from anywhere, but for Chapters 4 and beyond,
shaders are loaded from disk and the examples must be run from the chapter's
build directory (i.e. `cd build/src/chapterX/ && ./chapterX`)

