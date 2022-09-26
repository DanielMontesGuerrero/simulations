# simulations
[![build](https://github.com/DanielMontesGuerrero/simulations/actions/workflows/build.yml/badge.svg)](https://github.com/DanielMontesGuerrero/simulations/actions/workflows/build.yml)

Simulaciones de la materia `Computing Selected Topics`

## Lista de simulaciones
- [Conway's Game Of Life](https://github.com/DanielMontesGuerrero/simulations/tree/main/gameoflife)


#### Instalar dependencias
```bash
# Instalar dependencias
sudo apt update && sudo apt install -yq curl openjdk-8-jre-headless python2.7 gcc libsdl2-dev golang libcurlpp-dev libicu-dev
# Instalar buck
curl -L https://github.com/facebook/buck/releases/download/v2022.05.05.01/buck.2022.05.05.01_all.deb -o buck.deb
sudo dpkg -i buck.deb
sudo rm buck.deb
# Instalar buckaroo
curl -L https://github.com/LoopPerfect/buckaroo/releases/download/v2.2.0/buckaroo-linux -o /bin/buckaroo
chmod +x /bin/buckaroo
```
