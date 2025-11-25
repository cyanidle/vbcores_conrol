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
