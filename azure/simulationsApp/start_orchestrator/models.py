import dataclasses
from typing import List, Optional
import dataclasses_json
from start_orchestrator.config import BACK_END_PORT


@dataclasses.dataclass
class RequestData:
    rows: int
    cols: int


@dataclasses.dataclass
class WorkerInfo:
    rows: int
    cols: int
    host: str = '0.0.0.0'
    port: int = BACK_END_PORT
    protocol: str = 'tcp'


@dataclasses.dataclass
class OrchestratorInfo:
    rows: int
    cols: int
    host: str = '0.0.0.0'
    hosts: List[str] = dataclasses.field(default_factory=list)
    ports: List[int] = dataclasses.field(default_factory=list)
    protocol: str = 'tcp'
    port: int = BACK_END_PORT

@dataclasses_json.dataclass_json
@dataclasses.dataclass
class ComputeNodeInfo:
    public_ip: str
    port: int

@dataclasses_json.dataclass_json
@dataclasses.dataclass
class ResponseData:
    orchestrator: ComputeNodeInfo
    workers: List[ComputeNodeInfo] = dataclasses.field(default_factory=list)

@dataclasses_json.dataclass_json
@dataclasses.dataclass
class Response:
    result: bool
    description: str
    data: Optional[ResponseData]
