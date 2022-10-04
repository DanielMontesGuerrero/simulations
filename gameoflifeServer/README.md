# Game Of Life (distributed)

## Uso
### Orchestrator
En la raíz del projecto ejecuta:

```bash
buck run ////gameoflifeServer:orchestrator -- <args>
```

Los  argumentos soportados para el orchestrator son:
```txt
Usage of orchestrator:
  -cols int
        Number of cols for the orchestrator (default 10)
  -debug
        If set, doesn't print to Stdout
  -help
        Show usage
  -host string
        The host ip for the orchestrator (default "localhost")
  -hosts string
        The host ip list for the workers (default "localhost,localhost,localhost,localhost")
  -port int
        The port for the orchestrator (default 3000)
  -ports string
        The ports list for the workers (default "8080,8081,8082,8083")
  -protocol string
        The protocol to use (default "tcp")
  -rows int
        Number of rows for the orchestrator (default 10)
  -shouldModule
        If set, will module cell indexes when updating
  -standalone
        If set, the orchestrator will update workers periodically without waiting for GameHandler
```

### Worker
En la raíz del projecto ejecuta:

```bash
buck run //gameoflifeServer:worker -- <args>
```

Los argumentos soportados son:

```txt
Usage of worker:
  -cols int
        Number of cols for the worker (default 5)
  -debug
        If set, doesn't print to Stdout
  -help
        Show usage
  -host string
        The host ip for the worker (default "localhost")
  -port int
        The port for the worker (default 8080)
  -protocol string
        The protocol for the worker (default "tcp")
  -rows int
        Number of rows for the worker (default 5)
```


#### Ejemplo de uso
Para ejecutar un juego de tamaño 11x11 con distribuido en cuatro nodos ejecutaríamos lo siguiente:

```bash
# Worker1 - encargado de la submatriz superior izquierda
buck run //gameoflifeServer:worker -- --rows=5 --cols=5 \
  --host=0.0.0.0 --port=<worker1_port>
```

```bash
# Worker2 - encargado de la submatriz superior derecha
buck run //gameoflifeServer:worker -- --rows=5 --cols=6 \
  --host=0.0.0.0 --port=<worker2_port>
```

```bash
# Worker3 - encargado de la submatriz inferior izquierda
buck run //gameoflifeServer:worker -- --rows=6 --cols=5 \
  --host=0.0.0.0 --port=<worker3_port>
```

```bash
# Worker4 - encargado de la submatriz inferior derecha
buck run //gameoflifeServer:worker -- --rows=6 --cols=6 \
  --host=0.0.0.0 --port=<worker4_port>
```

```bash
# Orchestrator - encargado de sincronizar las actualizaciones de los nodos worker
buck run //gameoflifeServer:orchestrator -- --rows=11 --cols=11 \
  --host=0.0.0.0 --port=<orchestrator_port> \
  --hosts=<worker1_ip,worker2_ip,worker3_ip,worker4_ip> \
  --ports=<worker1_port,worker2_port,worker3_port,worker4_port>
```
