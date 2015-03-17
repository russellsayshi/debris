# debris
A trash manager for Linux

For those of you who like to work in the command line most of the time, I present the Debris trash manager, written in C.

(note that this may have undesirable results on non-ext4 filesystems)

##Usage

As of now, this comes with two built in commands, `rmt` and `debris`.

###debris

The debris command takes no arguments, and presents the user interface where the user can browse the contents of the trash, empty it, restore files, or whatever they may desire.

Pressing `h` or `?` will bring up the help menu.

The contents (as of version 1.0) of the help menu will look like this:

```
Help: [Q]uit
UP/DOWN: Moves
h/?: Help (this page)
q: Quit
RIGHT: select directory
r/Enter: Restore
l: Reload file list
LEFT: Up directory (if applicable)
i: File information
p: Display path to trash
e: Empty trash
c: Open config
```

Note that while the user can browse the contents of subdirectories, the user can't restore them individually, as the user can only restore top-level items. This may change in the future.

###rmt

The other command, rmt, puts a file in the trash.

Its usage is fairly simple: `rmt <filenames...>`

Specifying `rmt -h` or `rmt --help` will display the help.

##Configuration

The Debris UI is somewhat configurable, with the configuration file located at `~/.debris.config`.

The default config (created upon first run) will look like this:
```
[Debris Config]
ConfirmPrompt: 1
TrashColor: CYAN BLACK
SelectionColor: BLUE BLACK
DirectoriesColor: YELLOW BLACK
#Colors can be BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
```
The value of ConfirmPrompt should be either 0 or 1, and the colors should be two values, the first the foreground color and the second the background color.

Pressing `c` while in the Debris application will bring up the config file in the user's default editor.

Note that the config will not reload after exiting, and you must close and re-open the application to see the effect.

##Screenshots
Normal operation:

![Alt text](http://i.imgur.com/XjEg9yr.png "Default screen")

Viewing file metadata:

![Alt text](http://i.imgur.com/SRslncE.png "Viewing file metadata")
