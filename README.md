rtdgui
========
Spencer Hatch
May 7, 2014 12:40pm

This is the... rtdgui code. Note that you must install the (included) FLU libraries! See the README for the complex_proc repository for an example of how to use this thing. I don't know how to use it myself and no one has documented it, so I am just pointing you in a possible direction...


A note on compiling the FLU library (Jun 23, 2014)
---------------------------------------------------
If you have issues (you certainly will) compiling the FLU library, here are a few hot tips:
*Just comment out the overloading functions that cause errors in Flu_Tree_Browser.h. You'll know which ones when you compile.
*Modify $CXXFLAGS in 'makeinclude' by making -fpermissive one of its options. This will generate a ton of warnings, but you'll be in the clear, baby.
