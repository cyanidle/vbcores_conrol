# Пакеты
```bash
sudo apt install \
  openocd gcc-arm-none-eabi ninja-build cmake \
  libboost-all-dev
```

# Сборка
```bash
cmake -B build -G Ninja
ninja -C build
```

# Сборка для Малины на х64 ПК
* Скачиваем и распаковываем образ turtlebro2 https://disk.yandex.ru/d/fwXInv5GtNlwPg
* Сохраняем только папки `/usr` `/lib` `/include` (с сохранением атрибутов! - `cp -ra ... ~/sysroot`)
* Ставим кросс-компилятор для малины:
```bash
sudo apt install gсс-aarch64-linux-gnu g++-aarch64-linux-gnu
```
* Конфигурируем и собираем
```bash
cmake -B build -G Ninja -D TURTLEBRO2_SYSROOT=~/sysroot # Папка куда клали /usr
ninja -C build
```
* `build/vb_control` можно скопировать на малину и запустить