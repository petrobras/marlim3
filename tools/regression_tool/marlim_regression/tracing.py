from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
import platform


@dataclass
class TraceSupport:
    level: str
    available: bool
    explanation: str


class TraceProvider(ABC):
    @abstractmethod
    def support(self) -> TraceSupport: ...


class OptionalSystemTraceProvider(TraceProvider):
    def support(self) -> TraceSupport:
        system = platform.system()
        explanation = {
            "Linux": "Coverage is fully available. Detailed calls may be added through perf/eBPF when symbols and permissions allow.",
            "Darwin": "Coverage is fully available. Instruments-based call tracing is an optional future adapter.",
            "Windows": "Coverage is fully available. ETW-based call tracing is an optional future adapter.",
        }.get(system, "Coverage is available; no detailed trace adapter exists for this platform.")
        return TraceSupport("coverage-only", False, explanation)

