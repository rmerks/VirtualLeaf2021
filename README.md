# VirtualLeaf

VirtualLeaf is a cell-based computer modeling framework for simulating plant tissue morphogenesis.

## What is it?

**VirtualLeaf** is a cell-based tissue simulation software written in C++. Simulations proceed in dicreet steps where the system energy is minimized using a Monte-carlo approach. This approach generates realistic cell shapes and emergent tissue dynamics, enabling users to model a variety of developmental proccesses in plants.  
On the most basic level cell shape is defined through a balance of **turgor pressure**, causing expansion, and **cell wall tension**, causing restriction. By considering cell walls as springs that resist the internal pressure a mathematical description of the energy can be obtained. This expression, called the Hamiltonian, can also be extended to include other restrictions that a user may desire (e.g. cell shape).  
VirtualLeaf also comes with capabilities to perform **compartmentalized ODE-based simulations** that can affect the physical characterstics of the cell.  
This makes VirtualLeaf a powerful tool in understanding how plants grow. Try it out! 

    
![Alt text](/img/RootEmergence.jpg?raw=true "Optional Title")
## Getting Started

### Dependencies

* Describe any prerequisites, libraries, OS version, etc., needed before installing program.
* ex. Windows 10

### Installing

* How/where to download your program
* Any modifications needed to be made to files/folders

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

If you use VirtualLaef in a scientific publication please cite one of the following:  
-  [Enhanced Cell Wall Mechanics in VirtualLeaf Enable Realistic Simulations of Plant Tissue Dynamics](https://doi.org/10.1101/2024.08.01.605200)
-  [VirtualLeaf: An Open-Source Framework for Cell-Based Modeling of Plant Tissue Growth and Development](https://doi.org/10.1104/pp.110.167619)
-  [Building Simulation Models of Developing Plant Organs Using VirtualLeaf](https://doi.org/10.1007/978-1-62703-221-6_23)

