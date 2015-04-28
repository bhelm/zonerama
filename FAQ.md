# Zonerama does generate avis, but i cant stream them over zoneminder! #
streaming avis is not scope of Zonerama and its currently not supported by zoneminder.
one solutions is to create a samba/windows share and let your user use its own video player to watch the videos.

if you are a average php programmer, you could generate a flash based avi streaming plugin for zoneminder, let me know.

# I cant see a merged live stream! #

Zonerama is not a realtime streaming application, it generates merged events some minutes after the source events have been recorded. Use zoneminders "Montage" mode to view multiple monitors at once.

# i get a "qmake not found" error #
check your qt installation and path setup. you may refere to the wiki of your distribution to find out how to install qt.

# qmake/make gives me strange errors #
you have at least qt version 4.5 installed? then write a bug report with as much info about your system as possible.