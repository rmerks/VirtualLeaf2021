# VirtualLeaf

VirtualLeaf is a cell-based computer modeling framework for simulating plant tissue morphogenesis.

## What is it?

**VirtualLeaf** is a cell-based tissue simulation software written in C++. It simulates tissue dynamics through dicreet steps, minimizing the system energy using a Monte-carlo approach. This method generates realistic cell shapes and emergent tissue behaviors, allowing users to model a wide range of developmental proccesses in plants.  
At its core, cell shape is defined through a balance between **turgor pressure**, which drives expansion, and **cell wall tension**, which restricts it. By modeling cell walls as springs that resist the internal pressure, a mathematical description for the systems energy, known as the Hamiltonian, can be obtained. This expression can also be extended to include additional constraints, that the user defines (e.g. cell shape).  
VirtualLeaf also supports **compartmentalized ODE-based simulations**, enabling interactions between biochemical processes and physical properties of cells.  
These features make VirtualLeaf a powerful tool for exploring and understanding how plants grow and develop. Give it a try! 

    
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

If you use VirtualLeaf in a scientific publication please cite one of the following:  
-  [Enhanced Cell Wall Mechanics in VirtualLeaf Enable Realistic Simulations of Plant Tissue Dynamics](https://doi.org/10.1101/2024.08.01.605200)
-  [VirtualLeaf: An Open-Source Framework for Cell-Based Modeling of Plant Tissue Growth and Development](https://doi.org/10.1104/pp.110.167619)
-  [Building Simulation Models of Developing Plant Organs Using VirtualLeaf](https://doi.org/10.1007/978-1-62703-221-6_23)

