# xibridge

Кроссплатформенная библиотека для взаимодействия с устройствами uRPC и XIMС USB по протоколу XiNet.

* `server` - xibridge-сервер
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

Если планируется собирать xibridge_server, понадобится библитека libserialport. Для этого можно взять архив libserialport_release. Разархивированный 
каталог libserialport_release положить рядом с xibridge.

Библиотеку, сервер. тесты и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 2013, как и большинство наших 
проектов:

* cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);
* запустить cmake-gui;
* указать путь до каталога xibridge (с CMakeLists.txt) и путь <до каталога xibridge>/build в поле для выходной сборочной директории;
* Configure > Visual Studio 12 2013, x64;
* если нужен пример использования, поставить галочку BUILD_XIBRIDGE_EXAMPLE;
* если нужен xibridge_server, поставить галочку BUILD_XIBRIDGE_SERVER;
* Generate;
* собрать сгенерированное решение в Visual Studio

## Linux, Mac OS

Если планируется собирать xibridge_server, понадобится библитека libserialport. Разархивированный 
каталог libserialport_release положить рядом с xibridge.

Для Linux-систем установить пакет из папки с соответствующим именем (deb64 или deb_mips).

Библиотеку, тесты и пример собрать, как и все наши проекты на CMake\make:

* `mkdir build`
* `cd build`
* `cmake ..`
* если нужен пример использования, указать дополнительный параметр при запуске cmake: -DBUILD_XIBRIDGE_EXAMPLE=On;
* если нужен xibridge_server, указать дополнительный параметр при запуске cmake: -DBUILD_XIBRIDGE_SERVER=On;
* `make`

Для запуска сервера требуются права на доступ к последовательным портам в директории /dev (пользователь должен входить в группу dialout или быть root).
Сервер может принимать 3 параметра в следующем порядке: имя файла с ключами (по умолчанию используются встроенные ключи), опцию отладочного вывода (debug), 
способ  идентификации устройств (интерпретация идентификатора в протоколе) urpc, или ximc, или ximc_ext. По умочанию включается режим urpc. Все три параметра 
необязательны. Для работы в режиме urpc на unix-системах теперь не нужно создавать ссылки вида '/dev/ximc/<id>'. Сервер может полностью заменить собой 
urpc-xinet-сервер. Для этого он должен быть запущен в режиме urpc.

### Запуск тестов

Тестовый проект unitests (собирается вместе с библиотекой) представляет собой простую консольную программу, которая последовательно вызывает некоторые 
тест-функции, проверяющие совместимость данной библиотеки с действующими протоколами информационного обмена между xibridge-клиентом и
ximc-сервером, urpc-сервером, xibridge-сервером. Ее можно запустить, результаты и ход выполнения выводятся на экран.

### Пример использования

Пример использования example служит для демонстрации применения API библиотеки. Одновременно, этот пример можно использовать
для проверки работоспособности клиенткой библиотеки xibridge в реальном взаимодействии с ximc-xinet- и urpc-xinet- серверами.
Для успешной работы примера необходимо иметь в доступе сервер ximc-xinet и/или urpc-xinet, xibridge-. К каждому используемому серверу должен 
быть подключен по USB один контроллер (ximc в случае ximc-сервера, urmc - в случае urpc-сервера). Пример представляет собой 
консольное приложение, которое последовательно обращается  сначала к ximc-, затем urpc-серверу (при их наличии). Перед началом работы 
с конкретным типом сервера программа запрашивает его параметры (ip-адрес сервера, и номер подключенного по USB устройства, применеямый 
как идентификатор для данного типа сервера). В случае работы с xibridge-сервером  (третий по счету предлагаемый тип сервера в примере) после 
ip-адреса сервера запрашивается режим работы сервера (допускается - urpc, ximc или ximc_ext), затем идентификатор устройства (кроме случая 
ximc_ext - там выбирается первое из списка устройств, полученных через вызов enumerate-функции xibridge). Режим работы сервера, указанный в примере, 
должен совпадать с режимом работы, указанным в опциях командной строки при запуске программы xibridge_server.

## Выпуск релизов

Выпуск релиза:

* убедиться, что на windows, Linux и MacOS все собирается и работает;
* поменять версию (переменные XIBRIGE_VERSION_MAJOR, XIBRIGE_VERSION_MINOR, XIBRIDGE_VERSION_PATCH) в начале CMakeLists.txt;
* повесить тег vX.X.X на коммит, сделать push;
* написать changelog xibridge-x.x.x-changelog.txt;
* собрать библиотеку и xibridge-сервер под всеми архитектурами: win32, win64, debian linux, macos, бинарные результаты сборки добавить в 
архивы xibridge-x.x.x.zip и xibridge_server-x.x.x.zip в соответствующие папки;
* выложить их на github https://github.com/EPC-MSU/xibridge в разделе релизов с тегом vX.X.X;
* выложить xibridge-x.x.x-changelog.txt, xibridge-x.x.x.zip, xibridge_server-x.x.x..zip на сетевой диск.

## Генерация документации

Можно сгенерировать документацию к библиотеке. Для этого потребуется установить doxygen, если еще не установлен.  В каталоге с репозиторием 
запустить:

```bash
doxygen Doxyfile
```
