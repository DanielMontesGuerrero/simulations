# Conway's Game Of Life

![](../doc/img/board.jpg)

## Uso
En la raíz del projecto ejecuta:

```bash
buckaroo install
buck run gameoflife
```

## Controles
- &#8592;: Scroll a la izquierda
- &#8593;: Scroll hacia arriba
- &#8594;: Scroll a la derecha
- &#8595;: Scroll hacia abajo
- i: Zoom in
- o: Zoom out
- p: Pause
- click: Invierte el estado de la celda seleccionada
- f: Aumentar velocidad de animación
- s: Disminuir velocidad de animación
- u: Forzar actualización
- r: Forzar redibujado

## Modo de ejecución
### Ejecución local
La configuración necesaria del archivo [config.hpp](https://github.com/DanielMontesGuerrero/simulations/blob/main/gameoflife/include/config.hpp) para ejecutarlo en modo local es:
```cpp
const bool SHOULD_EXECUTE_LOCALLY = true;
```

### Ejecución distribuida
La configuración necesaria del archivo [config.hpp](https://github.com/DanielMontesGuerrero/simulations/blob/main/gameoflife/include/config.hpp) para ejecutarlo en modo distribuido es:
```cpp
const bool SHOULD_EXECUTE_LOCALLY = false;
//...
const bool IS_ORCHESTRATOR_STANDALONE = false;
```

#### Ejecución del orquestrador y los nodos worker
TBD...
