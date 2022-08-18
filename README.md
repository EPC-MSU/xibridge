# xibridge

Кроссплатформенная библиотека для взаимодействия с устройствами uRPC и XIMС USB по протоколу XiNet.

* `xibridge/server` - xibridge-сервер (в стадии проектирования и разработки)
* `xibridge/client` - библиотека API xibridge-клиента
* `xibridge/common` - общая часть
* `xibridge/test` - тесты внутренних функций проекта xibridge
* `xibridge/xiexample` - примеры использования API библиотеки

Для сборки проекта нужно сначала клонировать его:

```
git clone https://github.com/EPC-MSU/xibridge --recurse-submodules  
```

## Windows

Библиотеку, тестовый проект и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 2013, как и большинство наших 
проектов:

* cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);
* запустить cmake-gui;
* указать путь до каталога xibridge (с CMakeLists.txt) и такой же путь (до каталога xibridge) в поле для выходной сборочной директории;
* Configure > Visual Studio 12 2013, x64;
* если нужен тестовый проект, поставить галочку BUILD_TEST;
* если нужен пример использования, поставить галочку BUILD_XIEXAMPLE; 
* Generate;
* собрать сгенерированный проект в Visual Studio

## Linux

Библиотеку собрать, как и все наши проекты на CMake\make:

* `cmake CMakeLists.txt`
* если нужен тестовый проект, указать дополнительный параметр при запуске cmake: -DBUILD_TEST=On;
* если нужен пример использования, указать дополнительный параметр при запуске cmake: -DBUILD_XIEXAMPLE=On;
* `make`

## Замечания

Тестовый проект test представляет собой простую консольную программу, которая последовательно вызывает некоторые 
тест-функции, проверяющие совместимость данной библиотеки с действующиими протоколами информационного обмена между xibridge-клиентом и
ximc-сервером, urpc-серевром, xibridge-сервером. Ее можно запустить, результаты и ход выполения выводятся на экран.

Пример использования xiexample служит для демонстрации применения API библиотеки. Одновременно, этот пример можно использовать
для проверки работоспособности клиенткой библиотеки xibridge в реальном взаимодействии с ximc-xinet- и urpc-xinet- серверами.
Для успешной работы примера необходимо иметь в доступе сервер ximc-xinet и/или urpc-xinet. К каждому используемому серверу должен 
быть подключен по USB один контроллер (ximc в случае ximc-сервера, urmc - в случае urpc-сервера). Пример также представляет собой простое 
консольное приложение, которое последовательно обращается к ximc-, затем urpc-серверу (при их наличии). Перед началом работы 
с конкретным типом сервера программа запрашивает его параметры (ip-адрес сервера, и номер подключенного по USB устройства, 
применеямый как идентификатор для данного типа сервера). 

## Выпуск релизов

Выпуск релиза:

* Поменять версию VERSION в client/xibridge_client.h;
* В Windows окрыть решешие xibridge.sln и поменять версию продукта в проекте xibridge (version.rc); 
* Повесить тег vX.X.X на коммит, сделать push;
* Собрать библиотеку под всеми архитектурами: win32, win64, debian linux, бинарные результаты сборки добавить в архив в 
  соответствующие папки;
* Назвать этот архив xibridge-x.x.x.zip и выложить на github https://github.com/EPC-MSU/xibridge в разделе релизов с тегом v.X.X.X .
