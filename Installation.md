#how to install zonerama

# Details #

Installation is the same like on every other qt application. Make sure to read the [Requirements](Requirements.md) first.

  1. Download and extract source code
  1. **cd** to the source directory
  1. run **qmake**
  1. run **make**
  1. you now should have a **Zonerama** binary.
  1. create one or more merged monitor in Zoneminder:
    * Source: Local
    * Function: None
    * set width and height to equal outputImageHeight/Width
    * Other options are not needet, leave them default if unsure. So sourcepath /dev/video is okay, it isnt used anyway.
  1. edit **zonerama.conf** to fit your needs, read [Configuration](Configuration.md) carefully.
  1. configure your mysql server correctly with group\_concat\_max\_len and max\_allowed\_packet. See [Requirements](Requirements.md) for details.