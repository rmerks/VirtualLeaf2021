# VirtualLeaf

VirtualLeaf is a cell-based computer modeling framework for simulating plant tissue morphogenesis.

## What is it?

**VirtualLeaf** is a cell-based tissue simulation software written in C++. It simulates tissue dynamics through dicreet steps, minimizing the system energy using a Monte-carlo approach. This method generates realistic cell shapes and emergent tissue behaviors, allowing users to model a wide range of developmental proccesses in plants.  
At its core, cell shape is defined through a balance between **turgor pressure**, which drives expansion, and **cell wall tension**, which restricts it. By modeling cell walls as springs that resist the internal pressure, a mathematical description for the systems energy, known as the Hamiltonian, can be obtained. This expression can also be extended to include additional constraints, that the user defines (e.g. cell shape).  
VirtualLeaf also supports **compartmentalized ODE-based simulations**, enabling interactions between biochemical processes and physical properties of cells.  
These features make VirtualLeaf a powerful tool for exploring and understanding how plants grow and develop. Give it a try! 

    
![Example image showing how VirtualLeaf simulations look like](/img/RootEmergence.jpg?raw=true "VirtualLeaf demonstration")
## Getting Started
As of release v2.2.1, a precompiled binary of VirtualLeaf is available for Linux, Windows and macOS on the [GitHub Releases page](https://github.com/rmerks/VirtualLeaf2021/releases/tag/v2.2.1) - no installation is required beyond extracting the archive. Alternatively, you can manually compile VirtualLeaf yourself from source (e.g. if you want to build for another platform); instructions for this are given below.

VirtualLeaf v2.2 also introduces a `compatibility_level` option: a 16-bit bit-flag control parameter that lets you toggle new v2.2 features (the wall stiffness Hamiltonian, wall reconfiguration) on or off, to preserve backward compatibility with models built in older VirtualLeaf versions. The default value activates all features.

### Dependencies

VirtualLeaf requires the open source QT library, which requires a free QT account.
1. Register for a QT account [here](https://login.qt.io/login).
2. Download the [appropriate QT online installer](https://www.qt.io/download-qt-installer-oss) for your operating system.

#### Linux-specific setup
Before proceeding with the QT installation you may require additional packages. Run:
```console
sudo apt install libxcb-icccm4 libxkbcommon-x11-0 libxcb-image0 libxcb-keysyms1 libxcb-render-util0 libxcb-shape0
```

#### Installing QT
Now either follow the installation wizard or install QT via the command line (replace `PATH_TO_ONLINE_INSTALLER` with the actual path`:
```console
./PATH_TO_ONLINE_INSTALLER install qt6.8.0-essentials-dev  
```
-  Log in with your QT credentials
-  accept a bunch of licenses and follow the installation prompts
-  Remember the installation directory (default on Windows is `C:\QT`, on Linux `~/QT`)
-  It's normal to get some installation errors; it's usually fine to ignore them.

### Post-Installation Steps
#### Windows:
1.  Locate the folders that contain the `qmake` and `ming32-make` executables
    -  Look in `YOUR_QT_DIRECTORY/QT_VERSION/mingw_64/bin/`
    -  Alternatively, check `YOUR_QT_DIRECTORY/Tools/mingwSOMEVERSION/bin`
2.  Add the directories your PATH environment (see [here](https://www.youtube.com/watch?v=9umV9jD6n80))
3.  Re-launch your command line to apply the changes
4.  Verify that everything is correctly installed by running:
    ```console
    qmake --version
    mingw32-make --version
    ```
Now you should be all set for the installation!   

#### Linux:
1.  Verify `make` and `g++` are installed:
    ```console
    g++ --version
    make --version
    ```
    If missing install `make` and `g++` via:
    ```console
    sudo apt install make g++
    ```
2.  Locate the directory with the `qmake` executable. Should be located in `YOUR_QT_DIRECTORY/QT_VERSION/gcc_64/bin/`.
3.  Add the directory to your PATH (see [here](https://www.youtube.com/watch?v=jIunQSnzs1Y))
4.  To apply changes re-launch your command line or run
    ```console
    source ~/.bashrc
    ```
5.  Verify that everything is correctly installed by running:
    ```console
    g++ --version
    make --version
    qmake --version
    ```

Now you should be all set for the installation!   


### Installing

1.  Download the latest VirtualLeaf version:
    -  Clone the repository, or download the zip file [here](https://github.com/rmerks/VirtualLeaf2021/archive/refs/tags/v2.2.1.zip) for the version 2.2.1 release.
    -  If you downloaded the zip file, extract it into a folder of your choice.
    -  If you'd rather not compile VirtualLeaf yourself, a ready-to-run precompiled build is also available on the [v2.2.1 release page](https://github.com/rmerks/VirtualLeaf2021/releases/tag/v2.2.1).
2.  Navigate to the VirtualLeaf folder. The folder structure should look like this:  
    ```console
    └───VirtualLeaf2021-main
        ├───.github (contains GitHub workflows)
        │   └─
        ├───data (contains files for example models)
        │   └─
        ├───doc (contains documentation)
        ├───lib (contains required 3rd party packages)
        │   ├─
        └───src (contains the actual source code)
            ├─
    ```
3.  Navigate to the `src` folder and build VirtualLeaf:
    #### Windows
    Run:
    ```console
    qmake
    mingw32-make -f Makefile
    ```

    #### Linux
    Run:
    ```console
    qmake
    make -f Makefile
    ```
    If the installation fails you may be missing the OpenGL library. Install via:
    ```console
    sudo apt install freeglut3-dev
    ```

### Final Steps
After the build process is complete, you can find the VirtualLeaf executable in the `bin` folder:
```console
└───VirtualLeaf2021-main
    ├───.github (contains GitHub workflows)
    │   └─
    ├───data (contains files for example models)
    │   └─
    ├── bin
    │   ├─ VirtualLeaf
    ├───doc (contains documentation)
    ├───lib (contains required 3rd party packages)
    │   ├─
    └───src (contains the actual source code)
        ├─
    
```
You can now run VirtualLeaf!
### Executing program

Executing the VirtualLeaf executeable should bring up the GUI. Accept the license agreement and a base model should already be preloaded. 

<img src="/img/VirtualLeafOpen.png" alt="Displayed when opened." width="600">


You can start and stop simulations by pressing spacebar. You can also select one of the different preinstalled models by opening the menu at the top:

<img src="/img/VirtualLeafMenu.png" alt="Menu bar" width="600">

In this example we select the `Lateral Root Growth` model, which should now automatically be loaded:

<img src="/img/VirtualLeafLateralRoot1.png" alt="Lateral root model t=0" width="600">


Starting the simulation and letting it run for a bit generates something that should look similar to this:

<img src="/img/VirtualLeafLateralRoot2.png" alt="Lateral root model t=sometime" width="600">
 

Yay!

## svg-to-vl

`svg-to-vl` is a standalone Python command-line tool that converts hand-traced SVG tissue outlines into VirtualLeaf XML simulation templates. Requirements: Python >=3.10, pip, and Inkscape (for tracing the SVG from a microscopy image). Install it with:
```console
pip install svg-to-vl
```
See [src/python/README.md](src/python/README.md) for full usage instructions.

## Authors

Roeland Merks <Roeland.Merks@sysbio.nl>  
Richard van Nieuwenhoven <nieuwenhoven@iap.tuwien.ac.at>  
Ruth Großeholz <ruth.grosseholz@maastrichtuniversity.nl>  
Michael Guravage <M.A.Guravage@cwi.nl>  
Bruno Hay Mele  
Fabian Ormersbach  

## License

This project is licensed under the GPL-2.0 License - see the [license file](./COPYING) for details

## Citation

If you use VirtualLeaf in a scientific publication please cite one of the following:  
-  [Enhanced Cell Wall Mechanics in VirtualLeaf Enable Realistic Simulations of Plant Tissue Dynamics](https://doi.org/10.1101/2024.08.01.605200)
-  [VirtualLeaf: An Open-Source Framework for Cell-Based Modeling of Plant Tissue Growth and Development](https://doi.org/10.1104/pp.110.167619)
-  [Building Simulation Models of Developing Plant Organs Using VirtualLeaf](https://doi.org/10.1007/978-1-62703-221-6_23)

<!-- The bioRxiv preprint above is being superseded by a revised, updated manuscript covering VirtualLeaf-2.2. Update the citation entry with the journal DOI once the revision is published. -->

