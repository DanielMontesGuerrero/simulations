import dataclasses
from typing import List
import dataclasses_json


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

@dataclasses_json.dataclass_json
@dataclasses.dataclass
class ComputeNodeInfo:
    public_ip: str
    port: int

@dataclasses_json.dataclass_json
@dataclasses.dataclass
class RespponseData:
    orchestrator: ComputeNodeInfo
    workers: List[ComputeNodeInfo] = dataclasses.field(default_factory=list)
