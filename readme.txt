----- bfs2pack by Karok, release 1.2 ------

- GUI Mode -

-- Unpacks new bfs archives from flatout2 --

1. copy the content of this archive to some directory
2. start bfs2pack_gui.exe
3. drag and drop any bfs archive onto the bfs2pack window
4. choose a directory, where extracted files should be placed

-- Packs files into an bfs archive --

1. copy the content of this archive to some directory
2. start bfs2pack_gui.exe
3. drag and drop a directory onto the bfs2pack window (should contain the same structure like the flatout2-data-dir)
4. choose a filename (.bfs is added automatically)

5. edit file: filesystem, change it, so it looks like:
fo2a.bfs
fo2b.bfs
fo2c.bfs
<YOURNEWBFSHERE>.bfs
6. Notice: There MUST NOT be an empty last line!
7. Start flatout2, have fun!

- Console Mode -

Use bfs2pack_con.exe --help for some help. It basically works like any other archiver program (zip, tar, ...).

-- Note --

Every file is overwritten, WITHOUT any remark! 

----- History -----
06-09-08:
  - added console application

06-09-01:
  - http://sourceforge.net/projects/bfs2pack
  - small fixes

06-08-31:
  - included gfx by jelbo (www.flatoutjoint.com)
  - bugfixes

06-08-21:
  - new gui, with drag and drop support
  - packing original fo2a-data dir into a new bfs archive using the secure mode, creates a compatible archive!

06-08-19:
  - finally found the correct filename hashing algorithm (thx Pavcules)
  - now packing files with new filenames WORKS

06-08-16:
  - fixed some serious bugs (0 byte files and several problems with encrypting filenames)
  - extracted filename list to lua script (look at filenames.lua)
  - included two mods (fullroam and instantaction) to show usage and to prove functionality

06-07-18:
  - some fixes

06-07-17:
  - 3rd release
  - unpacking checks checksum (crc32)
  - packing, first test ;-)
  
06-07-13:
  - 2nd public release, now its c++
  
06-07-11:
  - 1. public release

----- Contact me -----

icq: #111595069

