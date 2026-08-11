====================
Building from Source
====================

Required Linux Dependencies
---------------------------

The following command installs all required dependencies using the ``apt`` package manager:

.. code-block:: bash

   sudo apt install cmake g++ libwayland-dev libwayland-bin wayland-protocols \
       pkg-config libxkbcommon-dev libxkbcommon-x11-dev libx11-dev \
       x11-xserver-utils libxrandr-dev libxinerama-dev libxcursor-dev \
       libxi-dev libgl-dev

Steps to build
-----

1. Clone the repository and navigate into the project directory:

   .. code-block:: bash

      git clone https://github.com/debbie-liknes/ChaiEngine.git
      cd ChaiEngine

2. Initialize and update git submodules:

   .. code-block:: bash

      git submodule update --init

3. Create and enter the build directory:

   .. code-block:: bash

      mkdir build && cd build

4. Generate the build files:

   .. code-block:: bash

      cmake ..

5. Open the visual solution *(Windows/MSVC)*:

   .. code-block:: bat

      start Chai.sln