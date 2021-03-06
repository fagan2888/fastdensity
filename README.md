# fastdensity
Density maps as fast as possible

## Getting started
these instructions will help you get a copy of the project, and make it run on your local machine for developpement and testing purpose. 
Please consult the licence of this project before.

### Prerequisites
What things you need for install the software, and where to find them, with some recommandation if you work with a windows OS)

#### With Linux/Mac OS :
**_NEED TO BE CONFIRMED_**
(with those OS the package manager inside should manage alone the link between all the files needed)

other softs :
* [Cmake](https://cmake.org/)
* G++ : C++11 required

librairies (just install them using the package manager of your OS):
* [GLEW](http://glew.sourceforge.net/)
* [GLFW](https://www.glfw.org/)
* [openCV](https://opencv.org/)
 

#### With Windows OS :
(working with windows may be slightly more complexe. Since there is no package manager include in the OS)

other softs :
* [Cmake](https://cmake.org/)
* G++ : C++11 required (I recommand using [Microsoft Visual Studio](https://visualstudio.microsoft.com/fr/) which provide G++ and got high compatibility with Cmake)

librairies (just install them using the following explanations):
* [GLEW](http://glew.sourceforge.net/)
* [GLFW](https://www.glfw.org/)
* [openCV](https://opencv.org/)

### Installing
A step by step series of instructions that tell you how to get a development environnement running.

#### With Linux/Mac OS :
**_TODO_**

#### With Windows OS :
 1. First, "git clone" somewhere on your machine 
 2. Then download every libraries in a static way, and store them somewhere (see the prerequisites)
 3. Create a "build" folder wherever you want, it will contain all the builds of your project. (I recommand inside the project "../fastdensity/build")
 4. Using Cmake (I recommand using the GUI in windows)  generate your build files.
		During the "configure" operation, you'll have to manually point where your libraries and include files are 
		(don't worry, if you're using the GUI we showed it up)
 5. build the programme, then before launching it, in "../build/src/release" copy all the .lib and .dll files from the libraries previously installed, and remimaps.vs & remimaps.fs.
 6. Execute the programme, selectiing main.exe as the solution to run. A .csv file called "bench_YYYY-MM-DD_HH-MM-SS" (where YYYY-MM-DD_HH-MM-SS represent the actual date and time) should have been generated in "../build/src", this file is the result of the run.

## Autors
* Jean-Daniel Fekete ( [Homepage](https://www.aviz.fr/~fekete/) )- Leader of the [INRIA](https://www.inria.fr/) Project Team [AVIZ](https://www.aviz.fr/)
* Frederic Vernier ( [Homepage](http://vernier.frederic.free.fr/) ) - Lecturer (Professor Assistant) teaching at [South-Paris University](http://www.u-psud.fr/fr/index.html) and researcher (at [LIMSI](https://www.limsi.fr/en/)).
* Rémi Bardes - Master student in computer science at [South-Paris University](http://www.u-psud.fr/fr/index.html)

## Licence :
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
