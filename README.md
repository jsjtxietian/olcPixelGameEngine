olcPixelGameEngine.h 

+-------------------------------------------------------------+
|           OneLoneCoder Pixel Game Engine v0.1               |
| "Like the command prompt console one, but not..." - javidx9 |
+-------------------------------------------------------------+

What is this?
~~~~~~~~~~~~~
The olcConsoleGameEngine has been a surprsing and wonderful
success for me, and I'm delighted how people have reacted so
positively towards it, so thanks for that.

However, there are limitations that I simply cannot avoid.
Firstly, I need to maintain several different versions of
it to accommodate users on Windows7, 8, 10, Linux, Mac,
Visual Studio & Code::Blocks. Secondly, this year I've been
pushing the console to the limits of its graphical capabilities
and the effect is becoming underwhelming. The engine itself
is not slow at all, but the process that Windows uses to
draw the command prompt to the screen is, and worse still,
it's dynamic based upon the variation of character colours
and glyphs. Sadly I have no control over this, and recent
videos that are extremely graphical (for a command prompt :P )
have been dipping to unacceptable framerates. As the channel
has been popular with aspiring game developers, I'm concerned
that the visual appeal of the command prompt is perhaps 
limited to us oldies, and I dont want to alienate younger
learners. Finally, I'd like to demonstrate many more
algorithms and image processing that exist in the graphical
domain, for which the console is insufficient.

For this reason, I have created olcPixelGameEngine! The look
and feel to the programmer is almost identical, so all of my
existing code from the videos is easily portable, and the
programmer uses this file in exactly the same way. But I've
decided that rather than just build a command prompt emulator,
that I would at least harness some modern(ish) portable 
technologies.

As a result, the olcPixelGameEngine supports 32-bit colour, is
written in a cross-platform style, uses modern(ish) C++
conventions and most importantly, renders much much faster. I
will use this version when my applications are predominantly
graphics based, but use the console version when they are
predominantly text based - Don't worry, loads more command
prompt silliness to come yet, but evolution is important!!

License (OLC-3)
~~~~~~~~~~~~~~~

Copyright 2018 OneLoneCoder.com

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

1. Redistributions or derivations of source code must retain the above 
copyright notice, this list of conditions and the following disclaimer.

2. Redistributions or derivative works in binary form must reproduce 
the above copyright notice. This list of conditions and the following 
disclaimer must be reproduced in the documentation and/or other 
materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its 
contributors may be used to endorse or promote products derived 
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Links
~~~~~
YouTube:	https://www.youtube.com/javidx9
Discord:	https://discord.gg/WhwHUMV
Twitter:	https://www.twitter.com/javidx9
Twitch:		https://www.twitch.tv/javidx9
GitHub:		https://www.github.com/onelonecoder
Homepage:	https://www.onelonecoder.com

Relevant Videos
~~~~~~~~~~~~~~~


Compiling in Linux
~~~~~~~~~~~~~~~~~~
You will need a modern C++ compiler, so update yours!
To compile use the command: 

g++ -o YourProgName YourSource.cpp -lX11 -lGL -lpthread -lpng

Compiling in Code::Blocks on Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Well I wont judge you, but make sure your Code::Blocks installation
is really up to date - you may even consider updating your C++ toolchain
to use MinGW32-W64, so google this. You will also need to enable C++14
in your build options, and add to your linker the following libraries:
user32 gdi32 opengl32 gdiplus

Thanks
~~~~~~
I'd like to extend thanks to Eremiell, slavka, Phantim, JackOJC, 
KrossX, Huhlig & MagetzUb for advice, ideas and testing, and I'd 
like to extend my appreciation to the 13K YouTube followers 
and 1K Discord server members who give me the motivation to keep 
going with all this :D

Author
~~~~~~
David Barr, aka javidx9, OneLoneCoder 2018
