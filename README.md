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

или скачать полный набор исходников(xibridge-src.zip) в разделе Releases. 

Заметка: `--recurse-submodules ` необходим для клонирования зависимостей: [Bindy](https://github.com/EPC-MSU/Bindy), [acutest](https://github.com/mity/acutest), [zf_log](https://github.com/wonder-mice/zf_log) и [tinycthread](https://github.com/tinycthread/tinycthread).

## Как собрать

### Windows

Если планируется собирать `xibridge_server`, понадобится библитека `libserialport`, разработанная проектом [sigrok](https://sigrok.org/wiki/Main_Page). Для этого можно взять архив `libserialport_release` из [раздела релизов репозитория](https://github.com/EPC-MSU/xibridge/releases) или с сетевого диска (`Z:\ximc\Xidcusb\Релизы\xibridge`). Разархивированный каталог libserialport_release положить рядом с получившимся каталогом `xibridge`.

Библиотеку, сервер, тесты и пример использования можно собрать, сначала с помощью CMake GUI, потом с помощью Visual Studio 2013, как и большинство наших проектов:

* cкачать и установить cmake (https://cmake.org) и MS Visual Studio (13 и выше);
* запустить cmake-gui;
* указать путь до каталога xibridge (с CMakeLists.txt) и путь <до каталога xibridge>/build в поле для выходной сборочной директории;
* Configure > Visual Studio 12 2013, x64;
* если нужен пример использования, поставить галочку BUILD_XIBRIDGE_EXAMPLE;
* если нужен xibridge_server, поставить галочку BUILD_XIBRIDGE_SERVER;
* Generate;
* собрать сгенерированное решение в Visual Studio

Замечание: режим BUILD_TEST позволяет собрать симулятор сервера и тесты. Используется для выявления проблем во время разработки.

### Linux, Mac OS

Если планируется собирать `xibridge_server`, понадобится библитека `libserialport`, разработанная проектом [sigrok](https://sigrok.org/wiki/Main_Page). Для этого можно взять архив `libserialport_release` из [раздела релизов репозитория](https://github.com/EPC-MSU/xibridge/releases) или с сетевого диска (`Z:\ximc\Xidcusb\Релизы\xibridge`). Разархивированный каталог libserialport_release положить рядом с получившимся каталогом `xibridge`. Для Linux-систем (debian и debian mipsel) установить пакет из папки с соответствующим именем (deb64 или deb_mips).

Библиотеку, тесты и пример собрать, как и все наши проекты на CMake\make:

* `mkdir build`
* `cd build`
* `cmake ..`
* если нужен пример использования, указать дополнительный параметр при запуске cmake: -DBUILD_XIBRIDGE_EXAMPLE=On;
* если нужен xibridge_server, указать дополнительный параметр при запуске cmake: -DBUILD_XIBRIDGE_SERVER=On;
* `make`

Замечание: режим BUILD_TEST позволяет собрать симулятор сервера и тесты. Используется для выявления проблем во время разработки.

## Как пользоваться

### Использование xibridge-сервера

xibridge_server - консольное приложение, предоставляющее возможность обращаться к контроллеру по сети. Это работает примерно так: имеется компьютер с известным ip и контроллерами, которые подключены к этому компьютеру по usb. На компьютере запущено приложение `xibridge_server`. Сервер может принимать на вход несколько параметров (порядок важен), см. 'Запуск xibridge-сервера'.

Пример команд запуска сервера:

```
./xibridge_server by_serial
```

```
./xibridge_server debug by_serial
```

Можно получить подсказку по набору принимаемых параметров, если ввести:

```
./xibridge_server help
```

Обращаться к контроллерам на сервере можно с помощью url такого вида `xi-net://<ip-адрес или имя сервера>/<id контроллера>`, например:
`xi-net://172.16.1.2/3` или `xi-net://serv.com/1f33`. В качестве id контроллера может выступать 1) серийный номер устройства (сервер должен быть 
запущен с опцией by_serial), 2) номер компорта под windows и адрес usb под линукс (опция сервера by_com_addr); 3) номер слота 
(опция сервера bvvu); 4)комбинированный номер vid-pid-serial, например `34d00207f2d5b4899` - здесь старшие два байта(4 цифры) - vid, затем 2 байта - pid, 
последние 4 байта - серийный номер (опция сервера by_serialpidvid).

Подробнее опции сервера рассмотрены ниже.

### Запуск xibridge-сервера

Если планируется только запускать xibridge_server, нужно взять релиз сервера в разделе "Releases". Для работы сервера требуется библиотека libserialport, которая
есть в архиве с релизом сервера под все платформы. 

#### Windows

Чтоб запустить сервер на windows, нужно взять каталог win32 или win64 в архиве с релизом сервера и просто 
запустить приложение `xibridge_server <опции>`  из этого каталога. 

#### Linux

Для запуска на линукс debian 64 или debian mipsel сначала взять из  архива libsearialport_release 
пакет `libserialport.deb` (в каталоге платформы применения) и установить его так: 

```
* `cd <путь до пакета>`
* `sudo apt install ./libserialport.deb`
```

Затем, скопировать `libxibridge_server_lib.so` в `/usr/local/lib`:

```
* `sudo cp <путь до релиза>/libxibridge_server_lib.so /usr/local/lib/libxibridge_server_lib.so`
```

Затем можно запускать xibridge-сервер, взятый из соответствующего каталога релиза. Под linux для работы сервера требуются права на доступ к последовательным портам в 
директории /dev (пользователь должен входить в группу dialout или быть root).

#### Mac Os

Для запуска сервера на Mac OS нужно: скопировать релизы библиотек `libserialport.0.dylib` и `libxibridge_server_lib.dylib` в системную папку `/usr/local/lib`:

```
* `sudo cp <путь до релиза>/libserialport.0.dylib /usr/local/lib/libserialport.0.dylib`
* `sudo cp <путь до релиза>/libxibridge_server_lib.dylib /usr/local/lib/libxibridge_server_lib.dylib`
```

Затем, взять релиз сервера, разрешить в системных настройках выполнение для сервера и скопированной библиотеки. 
Под Mac OS допустимые опции по способу идентификации устройств - `by_serial` и `by_serialpidvid`.

Сервер может принимать 3 параметра в следующем порядке: имя файла с ключами (по умолчанию используются встроенные ключи), опцию отладочного вывода (debug), 
способ  идентификации устройств (интерпретация идентификатора в протоколе).

Система идентификации устройств по номеру предполагает 4 подхода, которые задаются следующим набором опций:

* `by_serial` -  в качестве идентификатора используется серийный номер устройства;
* `by_com_addr` - в качестве идентификатора используютя либо номер com-порта (в ОС Windows), либо USB-адрес (для Linux), при этом считается, что все рабочие 
  устройства расположены на одной шине USB;
* `bvvu` - это режим по умолчанию, идентификатор формируется как 
  <номер конечного хаба>-1)*4+<номер USB на хабе>, если получившееся число больше 9, к нему прибавляется 6;
* `by_serialpidvid` - в качестве идентификатора используется комбинированный идентификтор из 12 байт (где старшие 4 байта - 0, следующие два байта - VID,
  затем 2 байта - PID, затем младшие 4 байта - серийный номер). 

По умолчанию включается режим bvvu. Все три параметра необязательны для всех платформ кроме Mac Os.  Под Mac OS для успешной работы приложения требуется явно 
указать тип идентификации - в Mac OS поддерживается `by_serial` или `by_serialpidvid`.

Сервер может полностью заменить собой urpc-xinet-сервер. Для работы в режиме bvvu на unix-системах теперь не нужно создавать ссылки вида '/dev/ximc/<id>'.
Для этого на БВВУ он должен быть запущен в режиме bvvu.

Идентификаторы устройств в  ответе на команду энумерации формируются в соответствии с выбранным подходом.

### Запуск тестов

Тестовый проект unitests (собирается вместе с библиотекой) представляет собой простую консольную программу, которая последовательно вызывает некоторые 
тест-функции, проверяющие совместимость данной библиотеки с действующими протоколами информационного обмена между xibridge-клиентом и ximc-сервером, urpc-сервером, 
xibridge-сервером. Ее можно запустить, результаты и ход выполнения выводятся на экран.

### Пример использования

Пример использования example служит для демонстрации применения API библиотеки. Одновременно, этот пример можно использовать
для проверки работоспособности клиенткой библиотеки xibridge в реальном взаимодействии с ximc-xinet-, urpc-xinet-, xibridge- серверами.
Для успешной работы примера необходимо иметь в доступе сервер ximc-xinet и/или urpc-xinet, xibridge-. К каждому используемому серверу должен 
быть подключен по USB один контроллер (ximc в случае ximc-сервера, urmc - в случае urpc-сервера). xibridge-Пример представляет собой 
консольное приложение, которое последовательно обращается  сначала к ximc-, затем urpc-серверу и xibridge-серверу(при их наличии). Перед началом работы 
с конкретным типом сервера программа запрашивает его параметры (ip-адрес сервера,  идентификатор устройства для данного типа сервера). 

В случае работы с xibridge-сервером (третий по счету предлагаемый тип сервера в примере) после ip-адреса сервера запрашивается тип идентификации устройств 
(допускается - bvvu, by_com_addr, by_serial или by_serialpidvid), затем идентификатор устройства (кроме случая by_serialpidvid - там выбирается первое из списка 
устройств, полученных через вызов enumerate-функции xibridge). Тип идентификации устройств, указанный в примере, должен совпадать с типом идентификации, указанным 
в опциях командной строки при запуске программы xibridge_server.

## Выпуск релизов

Выпуск релиза:

* убедиться, что на windows, Linux и MacOS все собирается и работает;
* поменять версию (переменные XIBRIGE_VERSION_MAJOR, XIBRIGE_VERSION_MINOR, XIBRIDGE_VERSION_PATCH) в начале CMakeLists.txt;
* повесить тег vX.X.X на коммит, сделать push;
* написать changelog xibridge-x.x.x-changelog.txt;
* собрать библиотеку и xibridge-сервер под всеми архитектурами: win32, win64, debian linux, macos, бинарные результаты сборки добавить в 
  архивы xibridge.zip и xibridge_server.zip в соответствующие папки;
* для win32 и win64 в каталог с релизом положить также собранный релиз `libserialport.dll` под соответствующую архитектуру (win32 или win64);
* для debian64 и debian mipsel в каталог с релизом xibridge-сервера положить пакет с библиотекой `libserialport.deb`;
* для mac os положить в каталог с релизом библиотеку `libserialport.0.dylib`
* выложить их на github https://github.com/EPC-MSU/xibridge в разделе релизов с тегом vX.X.X;
* туда же выложить libserialport_release.zip последней версии;
* выложить xibridge-x.x.x-changelog.txt, xibridge.zip, xibridge_server.zip на сетевой диск, переименовав при этом xibridge.zip в xibridge-x.x.x.zip,
  xibridge_server.zip в xibridge_server-x.x.x.zip.

## Генерация документации

Можно сгенерировать документацию к библиотеке. Для этого потребуется установить doxygen, если еще не установлен.  В каталоге с репозиторием 
запустить:

```bash
doxygen Doxyfile
```
