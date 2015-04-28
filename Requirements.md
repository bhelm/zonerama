#Requirements for installing Zonerama

# Details #

  * A working Zonerama Installation, or at least the Events-Dir, the Zoneminder Database and ffmpeg
  * Qt 4.5 or newer
  * a working C++ Compiler (gcc)
  * A Fast System to deploy, expecially when it comes to hard disk read and write.
  * MySql-Server configured with large max\_allowed\_packet and group\_concat\_max\_len. the reason for this is that zonerama does a sql query with a very large result set. the size of the result set depends on the number of monitors to merge, the frames per second per monitor and the maxLength parameter. for example, put into /etc/my.cnf or /etc/mysql/my.cnf below the "mysqld"-section
    * max\_allowed\_packet = 20M
    * group\_concat\_max\_len = 10M