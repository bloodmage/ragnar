# Ragnar (64bit - libtorrent 1.0.6) 
Ragnar is a C++/CLI wrapper for Rasterbar's *libtorrent*. It aims to provide a (mostly) complete interface to the underlying C++ library.


## How to build from source
The repository includes a `bootstrap.bat` file which runs the `run-bootstrap.ps1` script. This will download and compile both Boost and libtorrent automatically.

### 1. Run the `bootstrap.bat` file. If it hangs after "Bootstrapping Boost" - then simply run it again.

### 2. Building Ragnar
Open `Ragnar.sln` in Visual Studio 2013. Press `F6`. Wait a while. Success. If not - report an issue.

## Contributing
* Fork the repository.
* Make your feature addition or bug fix.
* Send a pull request. *Bonus for topic branches. Funny .gif will be your reward.*

## License
Copyright (c) 2014, Viktor Elofsson and contributors

Ragnar is provided as-is under the **MIT** license. For more information see `LICENSE`.

*For Boost, see http://svn.boost.org/svn/boost/trunk/LICENSE_1_0.txt*

*For libtorrent, see https://libtorrent.googlecode.com/svn/trunk/LICENSE*
