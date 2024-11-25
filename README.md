# VirtualLeaf

VirtualLeaf is a cell-based computer modeling framework for simulating plant tissue morphogenesis.

## What is it?

**VirtualLeaf** is a cell-based tissue simulation software written in C++. It simulates tissue dynamics through dicreet steps, minimizing the system energy using a Monte-carlo approach. This method generates realistic cell shapes and emergent tissue behaviors, allowing users to model a wide range of developmental proccesses in plants.  
At its core, cell shape is defined through a balance between **turgor pressure**, which drives expansion, and **cell wall tension**, which restricts it. By modeling cell walls as springs that resist the internal pressure, a mathematical description for the systems energy, known as the Hamiltonian, can be obtained. This expression can also be extended to include additional constraints, that the user defines (e.g. cell shape).  
VirtualLeaf also supports **compartmentalized ODE-based simulations**, enabling interactions between biochemical processes and physical properties of cells.  
These features make VirtualLeaf a powerful tool for exploring and understanding how plants grow and develop. Give it a try! 

    
![Alt text](/img/RootEmergence.jpg?raw=true "Optional Title")
## Getting Started
Currently VirtualLeaf requires you to manually compile it yourself (don't worry, we'll walk you through the process). We plan on distributing a pre-compiled version in the future.

### Dependencies

VirtualLeaf requires the open source QT library, which requires an account (register [here](https://login.qt.io/login)).  
Download the [QT online installer](https://www.qt.io/download-qt-installer-oss) for your operating system. Now either follow the installation wizard or install QT via the command line:
```console
./PATH_TO_ONLINE_INSTALLER install qt6.8.0-essentials-dev  
```
Log in with your QT credentials and follow the installation prompts. Note the installation directory. After successfully installing QT you need to locate `qmake` and `ming32-make`. They should be located at `YOUR_QT_DIRECTORY/QT_VERSION/mingw_64/bin/` and/or `YOUR_QT_DIRECTORY/Tools/mingwSOMEVERSION/bin`. Add the directories in which you located both files to your PATH (for Windows see [here](https://www.youtube.com/watch?v=9umV9jD6n80) and for Linux see [here](https://www.youtube.com/watch?v=jIunQSnzs1Y)) and re-launch your command line.  
Now you should be all set for the installation!   


### Installing

Download the latest VirtualLeaf version either from GitHub, clone the repository, or simply click [here](https://github.com/rmerks/VirtualLeaf2021/archive/refs/tags/v2.0.0.zip) for the version 2.0.0 release. If you downloaded a zip file unzip it into a folder of your choice.  
You should now have the following folder structure:
```console
└───VirtualLeaf2021-main
    ├───.github
    │   └─
    ├───data
    │   └─
    ├───doc
    ├───lib
    │   ├─
    └───src
        ├─
```
Change directory to the `src` folder and invoke
```console
qmake
mingw32-make -f Makefile
```
This may take some time. After everything's finished you can find the VirtualLeaf executable in the `bin` folder.

### Executing program

* How to run the program
* Step-by-step bullets
```
code blocks for commands
```

## Help

Any advise for common problems or issues.
```
command to run if program contains helper info
```

## Authors

Roeland Merks <Roeland.Merks@sysbio.nl>  
Richard van Nieuwenhoven <nieuwenhoven@iap.tuwien.ac.at>  
Ruth Großeholz <ruth.grosseholz@maastrichtuniversity.nl>  
Michael Guravage <M.A.Guravage@cwi.nl>  

## Version History

* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

This project is licensed under the GPL-2.0 License - see the [license file](./COPYING) for details

## Citation

If you use VirtualLeaf in a scientific publication please cite one of the following:  
-  [Enhanced Cell Wall Mechanics in VirtualLeaf Enable Realistic Simulations of Plant Tissue Dynamics](https://doi.org/10.1101/2024.08.01.605200)
-  [VirtualLeaf: An Open-Source Framework for Cell-Based Modeling of Plant Tissue Growth and Development](https://doi.org/10.1104/pp.110.167619)
-  [Building Simulation Models of Developing Plant Organs Using VirtualLeaf](https://doi.org/10.1007/978-1-62703-221-6_23)

