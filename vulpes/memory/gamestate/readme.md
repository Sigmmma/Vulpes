# Gamestate

The gamestate folder is meant to house structures and memory getters that
are stored in gamestate. E.g., they are saved to checkpoint files and reside
in gamestate memory or upgraded gamestate.

It is bad business to put references to outside of gamestate in anything in
this memory as far as I know.
