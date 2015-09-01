rtdgui
========
Spencer Hatch
May 7, 2014 12:40pm

This is the... rtdgui code. Note that you must install the (included) FLU libraries! See the README for the complex_proc repository for an example of how to use this thing. As another example  (that is confirmed to be working!) this repository also includes two little bash scripts written by MPD for running both data acquisition and real-time display on Merry. They are updated so as to be functional with Merry as he stands in Jun 2014.

Do this too (you need all of these):
sudo apt-get install libfftw3-3 libfftw3-dev libfftw3-double3 libfltk1.3-dev libfltk-gl1.3 libfl-dev libxft-dev libjpeg8-dev libpng++-dev

A note on compiling the FLU library (Jun 23, 2014)
--------------------------------------------------
If you have issues (you certainly will) compiling the FLU library, here are a few hot tips:
*Just comment out the overloading functions that cause errors in Flu_Tree_Browser.h. You'll know which ones when you compile.
*Modify $CXXFLAGS in 'makeinclude' by making -fpermissive one of its options. This will generate a ton of warnings, but you'll be in the clear, baby.

Another note on compiling FLU (Sep 01, 2015)
--------------------------------------------
Note, on some machines that DO have fltk and GLU installed, you really do have to specify another directory for configure to look. In the most recent case (Elrond), I had to run configure like so:
./configure --L=/usr/lib/x86_64-linux-gnu/

Good luck.
