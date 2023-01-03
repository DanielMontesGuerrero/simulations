import math

from dataclasses import dataclass
from typing import List, Callable


@dataclass
class PlotMetadata:
    title: str
    xlabel: str
    ylabel: str
    formats: List[str]
    num_lines: int
    transform_funcs: List[Callable]
    labels: List[str]


density_metadata = PlotMetadata('Gráfica de la Densidad', 'Generación', 'Densidad', ['o-r', 'o-b'], 2, [
    lambda x: x,
    lambda x: math.log10(x) if x > 0 else 0,
], labels=['densidad', 'log10(densidad)'])
entropy_metadata = PlotMetadata('Gráfica de la Entropía', 'Generación', 'Entropía', ['o-b'], 1, [
    lambda x: x,
], labels=['entropía'])
