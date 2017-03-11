# atmega8 based controller for Toyota's headunit display block

I wanted to use panel from Toyota, so I created special controller that can work with it and send all its events
to another block using serial interface. I used Panasonic datasheet (you can find it in the root) to understand how 
the panel works. Controller works on 5V, but requires 10V input for backlight (if you need it).

## Panel's look:

![panel](https://habrastorage.org/files/0c4/a37/89c/0c4a3789ce8b442c8ab7ed8e6bc1b6d7.jpg)

## Schematic:

![schematic](https://habrastorage.org/files/d18/09f/c45/d1809fc456794baf8973b73bf9ca67bc.png)
