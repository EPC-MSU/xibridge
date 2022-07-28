#xibridge
Кроссплатформенная библиотека и сервер для взаимодействия с устройствами uRPC и XIMС USB по протоколу XiNet.

* `xibridge/client` - библиотека API xibridge-клиента
* `xibridge/misc` - общая часть`
* `xibridge/xitest` - тесты внутренних функций проекта xibridge
* `xibridge/xiexample` - примеры использования API библиотеки
  
Для сборки проекта сначала клонировать его:

> git clone https://github.com/EPC-MSU/xibridge --recurse-submodules  

## Windows

Библиотеку, тестовый проект и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 2013, как и большинство наших 
проектов:

> cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);

> запустить cmake-gui;

> указать путь до каталога xibridge и такой же (до каталога xibridge) до выходной сборочной директории;

> Configure > Visual Studio 12 2013, x64;

> если нужен тестовый проект, поставить галочку BUILD_XITEST;

> если нужен пример использования, поставить галочку BUILD_XIEXAMPLE; 

> Generate;

> собрать сгенерированный проект в Visual Studio

##Linux 

Библиотеку собрать, как и все наши проекты на CMake\make:

> cmake CMakeLists.txt;

> если нужен тестовый проект, указать дополнительный параметр при запуске cmake: -DBUILD_XITEST=On;

> если нужен пример использования, указать дополнительный параметр при запуске cmake: -DBUILD_XIEXAMPLE=On;
 
> make  


