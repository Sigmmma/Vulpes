# Introduction
These docs are about the internal working of Vulpes and why certain things
are designed the way they are.

There is a lot of nuance that goes into these decisions at times. And that
should not be lost.

## VirtualProtect
Virtual protect is needed because under certain circumstances it can be
important to make sure that when we're writing things to memory that that
memory is properly marked.

I said under certain circumstances because it can differ per environment.
On earlier versions of Windows this protection was not in place. It also
doesn't exist in wine. And it can be retroactively enabled by other mods
using a single Windows API function.

Some mods like Opensauce and HAC2 change the protection of the entire
executable to read-write-execute. This is bad, and we should never consider
doing this. This opens up attacks from other LUA APIs, malformed maps etc.
Not even for performance reasons should we consider doing this. If we are
constantly rewriting portions of code multiple times a second we're doing
something wrong.

## Why do we use three levels of abstractions for some pointers? (\*\*\*)

This is because our signatures point to code. So, we have a value that is
a reference to a piece of code (1). That piece of code references a pointer (2).
That last pointer is of course also a reference (3).

That is why we need three asterisks sometimes. It gets me confused a lot too.

The reason that we use signatures that point to code is because you can't have
reliable signatures for data outside of code. And we use signatures because
they are (if done right) mostly binary agnostic given that the targets were
compiled with similar enough settings. And it helps with situations
where another mod edits the code we need so we don't have an unreported error.
And we instead just fail and log that the signature wasn't found, because the
code was in a way unsafe.
