# Ragnar (32bit #DEFAULT# or 64bit - libtorrent 1.1.4) 
Ragnar is a C++/CLI wrapper for Rasterbar's *libtorrent*. It aims to provide a (mostly) complete interface to the underlying C++ library.

Now superchaged with hooking to libtorrent from .NET directly.

## How to build from source
The repository includes a `bootstrap.bat` file which runs the `run-bootstrap.ps1` script. This will download and compile both Boost and libtorrent automatically.

### 1. Run the `bootstrap.bat` file. If it hangs after "Bootstrapping Boost" - then simply run it again.
### HACK: Modify system &lt;atomic&gt; header
Open system `atomic` header and simply comment the error line at beginning

### 2. Building Ragnar
Open `Ragnar.sln` in Visual Studio 2013. Press `F6`. Wait a while. Success. If not - report an issue.

## Notice
If you want to build 64bit version, change `run-bootstrap.ps1`, uncomment 64 bit flags, and comment 32 bit flags. (line 92 and 103)

## Contributing
* Fork the repository.
* Make your feature addition or bug fix.
* Send a pull request. *Bonus for topic branches. Funny .gif will be your reward.*

## License
Copyright (c) 2014, Viktor Elofsson and contributors

Ragnar is provided as-is under the **MIT** license. For more information see `LICENSE`.

*For Boost, see http://svn.boost.org/svn/boost/trunk/LICENSE_1_0.txt*

*For libtorrent, see https://libtorrent.googlecode.com/svn/trunk/LICENSE*
