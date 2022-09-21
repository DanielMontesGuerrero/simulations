import dataclasses
from typing import List


@dataclasses.dataclass
class RequestData:
    rows: int
    cols: int


@dataclasses.dataclass
class WorkerInfo:
    rows: int
    cols: int
    port: int = 8080
    protocol: str = 'tcp'


@dataclasses.dataclass
class OrchestratorInfo:
    rows: int
    cols: int
    hosts: List[str] = dataclasses.field(default_factory=list)
    ports: List[int] = dataclasses.field(default_factory=list)
    protocol: str = 'tcp'
    port: int = 8080
