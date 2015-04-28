## How it works ##

After startup, Zonerama first parses all config files. after that, it starts processing the individual monitors. this starts at ...

### MergedMonitor::generateMergedScene() ###

The purpose of this function is to get frame information from the zoneminder database and syncronize them. How many frames (or better seconds) to get is determined by the maxLength option. Zonerama will always take the OLDEST frames from the linked monitors. so if you set the configuration values deleteMergedFramesFromDB and deleteMergedFramesFromHDD to false, Zonerama will always process the same frames again (useful for testing).

The result of this function is a so called MergedScene, a array of MergedFrames that contain information about which frames should be drawn together.

after that, it comes to...

### MergedMonitor::renderMergedScene() ###
this is the working horse of zonerama. it takes the mergedScene, reads the source files from the events dir, processes them into one "Merged" image and saves it to harddisk. it also deletes files and database entries if you configured it to do so.

### bool MergedMonitor::generateVideo() ###
if you have set generateVideo to true, this is called after the Merged Scene is rendered. this function launches ffmpeg with the right arguments. Note that you can modify the arguments in the configfile.