# debris
A trash manager for Linux

For those of you who like to work in the command line most of the time, I present the Debris trash manager, written in C.

(note that this may have undesirable results on non-ext4 filesystems)

##Usage

As of now, this comes with two built in commands, `rmt` and `debris`.

###debris

The debris command takes no arguments, and presents the user interface where the user can browse the contents of the trash, empty it, restore files, or whatever they may desire.

Pressing `h` or `?` will bring up the help menu.

Note that while you can browse the contents of subdirectories, you can't restore them individually, as you can only restore top-level items. This may change in the future.

###rmt

The other command, rmt, puts a file in the trash.

Its usage is fairly simple: `rmt <filenames...>`

Specifying `rmt -h` or `rmt --help` will display the help.

##Configuring

The Debris UI is somewhat configurable, with the configuration file located at `~/.debris.config`.

A sample config will look like this:
