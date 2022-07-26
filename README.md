# xibridge
Кроссплатформенная библиотека и сервер для взаимодействия с устройствами uRPC и XISM USB по протоколу XiNet.

* `xibridge_server` - сервер.
* `libxibridge` - библиотека
* `common` - общая часть

#xibridge
Кроссплатформенная библиотека и сервер для взаимодействия с устройствами uRPC и XIMС USB по протоколу XiNet.

* `xibridge/server` - сервер.
  `xibridge/client` - библиотека
  `xibridge` - общая часть`
  `xibridge/xitest` - тесты внутренних функций проекта xibridge
  `xibridge/xiexample` - примеры использования API библиотеки
  
Для сборки проекта сначала клонировать его:

> git clone https://github.com/EPC-MSU/xibridge --recurse-submodules  

## Windows

Библиотеку, тестовый проект и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 20<13, как и большинство наших 
проектов:

> cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);
> запустить cmake-gui;
> указать путь до исходного когда и до выходной сборочной директории (рекомендуется создать отдельную папки с именем build);
> Configure > Visual Studio 12 2013, x64;
> Если нужен тестовый проект, поставить галочку BUILD_XITEST;
> Если нужен пример использования, поставить галочку BUILD_XIEXAMPLE; 
> Generate;
> собрать сгенерированный проект в Visual Studio

##Linux 

Библиотеку собрать, как и все наши проекты на CMake\make:

> cmake CMakeLists.txt;

> make  


