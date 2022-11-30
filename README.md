# xibridge

Кроссплатформенная библиотека для взаимодействия с устройствами uRPC и XIMС USB по протоколу XiNet.

* `server` - xibridge-сервер (в стадии проектирования и разработки)
* `client` - библиотека API xibridge-клиента
* `common` - общая часть
* `test` - тесты внутренних функций проекта xibridge
* `example` - примеры использования API библиотеки

Для сборки проекта нужно сначала клонировать его:

```
git clone https://github.com/EPC-MSU/xibridge --recurse-submodules  
```

или скачать исходники (Windows) c помощью скрипта get_src.bat, запускать лучше всего в отдельном (новом) каталоге. 

## Windows

Библиотеку, тесты и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 2013, как и большинство наших 
проектов:

* cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);
* запустить cmake-gui;
* указать путь до каталога xibridge (с CMakeLists.txt) и путь <до каталога xibridge>/build в поле для выходной сборочной директории;
* Configure > Visual Studio 12 2013, x64;
* если нужен пример использования, поставить галочку BUILD_XIBRIDGE_EXAMPLE; 
* Generate;
* собрать сгенерированное решение в Visual Studio

## Linux, Mac OS

Библиотеку, тесты и пример собрать, как и все наши проекты на CMake\make:

* `mkdir build`
* `cd build`
* `cmake ..`
* если нужен пример использования, указать дополнительный параметр при запуске cmake: -DBUILD_XIBRIDGE_EXAMPLE=On;
* `make`

### Запуск тестов

Тестовый проект unitests (собирается вместе с библиотекой) представляет собой простую консольную программу, которая последовательно вызывает некоторые 
тест-функции, проверяющие совместимость данной библиотеки с действующими протоколами информационного обмена между xibridge-клиентом и
ximc-сервером, urpc-сервером, xibridge-сервером. Ее можно запустить, результаты и ход выполнения выводятся на экран.

### Пример использования

Пример использования example служит для демонстрации применения API библиотеки. Одновременно, этот пример можно использовать
для проверки работоспособности клиенткой библиотеки xibridge в реальном взаимодействии с ximc-xinet- и urpc-xinet- серверами.
Для успешной работы примера необходимо иметь в доступе сервер ximc-xinet и/или urpc-xinet. К каждому используемому серверу должен 
быть подключен по USB один контроллер (ximc в случае ximc-сервера, urmc - в случае urpc-сервера). Пример представляет собой 
консольное приложение, которое последовательно обращается  сначала к ximc-, затем urpc-серверу (при их наличии). Перед началом работы 
с конкретным типом сервера программа запрашивает его параметры (ip-адрес сервера, и номер подключенного по USB устройства, 
применеямый как идентификатор для данного типа сервера). 

## Выпуск релизов

Выпуск релиза:

* убедиться, что на windows, Linux и MacOS все собирается и работает;
* поменять версию (переменные XIBRIGE_VERSION_MAJOR, XIBRIGE_VERSION_MINOR, XIBRIDGE_VERSION_PATCH) в начале CMakeLists.txt;
* повесить тег vX.X.X на коммит, сделать push;
* написать changelog xibridge-x.x.x-changelog.txt;
* собрать библиотеку под всеми архитектурами: win32, win64, debian linux, macos, бинарные результаты сборки добавить в архив в 
соответствующие папки;
* назвать этот архив xibridge-x.x.x.zip и выложить на github https://github.com/EPC-MSU/xibridge в разделе релизов с тегом vX.X.X;
* выложить xibridge-x.x.x-changelog.txt, xibridge-x.x.x.zip на сетевой диск .

## Генерация документации

Можно сгенерировать документацию к библиотеке. Для этого потребуется установить doxygen, если еще не установлен.  В каталоге с репозиторием 
запустить:

```bash
doxygen Doxyfile
```
