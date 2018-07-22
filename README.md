Twitch Plasmoid
===============
A simple plasmoid showing you your favourite streamers. 

Features
--------
* Add, remove channels
* Display a notification whenever a streamer starts streaming
* Open live stream in browser or execute a custom command

Screenshot
----------
![Screenshot](/twitch-plasmoid.png)

Build instructions
------------------
In the project's root folder, run:
```bash
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX="/usr"
cmake --build . --config Release
sudo make install
```

Contribute
------------------
Bug reports and pull requests are highly appreciated.

License
------------------
GPLv3, see LICENSE.
