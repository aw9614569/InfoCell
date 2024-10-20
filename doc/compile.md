Compiling InfoCell

Dependencies are in Conan ( https://conan.io/ )

Ususally need a pip and conan upgrade before 3rd party update
  python.exe -m pip install --upgrade pip
  pip install conan --upgrade
Also sometimes you need a conan profile refresh
  conan profile detect

On Windows:
  in "..\ARC\conan\Debug"
  conan install ..\..\InfoCell --settings=build_type=Debug --build=missing -of .

  in "..\ARC\conan\Release"
  conan install ..\..\InfoCell --settings=build_type=Release -of .

On Linux:
  in ~/devel/conan/Release
  conan install ../../InfoCell/ --settings=build_type=Release -of .