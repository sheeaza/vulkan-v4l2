# vulkan-v4l2
- simple v4l2 capture based on vulkan.

## building
```sh
# for wayland
# install https://github.com/KDE/extra-cmake-modules to sdk's sysroot first

$ git clone https://github.com/sheeaza/vulkan-v4l2.git -b <branch-you-want>
$ cd vulkan-v4l2
$ mkdir build
$ cd build
$ source path/to/sdk        # for cross compiler
$ cmake ../
$ make -j<N> && make install
```
copy all the outputs to the board and run.
