# Copyright 2026 The MIND Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
MIND ctypes FFI bindings.

This module provides pure Python bindings to libmind using ctypes.
No external dependencies required.
"""

import ctypes
import ctypes.util
import os
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional, Union

# =============================================================================
# C Structure Definitions
# =============================================================================

class _CrConfig(ctypes.Structure):
    """Maps to cr_config_t"""
    _fields_ = [
        ("embedding_dim", ctypes.c_int),
        ("max_memory_slots", ctypes.c_int),
        ("initial_plasticity", ctypes.c_float),
    ]


class _CrHint(ctypes.Structure):
    """Maps to cr_hint_t"""
    _fields_ = [
        ("vector", ctypes.POINTER(ctypes.c_float)),
        ("dim", ctypes.c_int),
        ("confidence", ctypes.c_float),
    ]


class _CrPlasticity(ctypes.Structure):
    """Maps to cr_plasticity_t"""
    _fields_ = [
        ("plasticity", ctypes.c_float),
        ("stability", ctypes.c_float),
        ("age", ctypes.c_float),
    ]


class _CrTemporal(ctypes.Structure):
    """Maps to cr_temporal_t"""
    _fields_ = [
        ("age", ctypes.c_float),
        ("plasticity", ctypes.c_float),
        ("velocity", ctypes.c_float),
        ("maturity", ctypes.c_float),
        ("last_reinforcement_age", ctypes.c_float),
        ("time_since_reinforcement", ctypes.c_float),
        ("total_updates", ctypes.c_int),
        ("total_reinforcements", ctypes.c_int),
    ]


class _CrCalibration(ctypes.Structure):
    """Maps to cr_calibration_t"""
    _fields_ = [
        ("age", ctypes.c_float),
        ("plasticity", ctypes.c_float),
        ("velocity", ctypes.c_float),
        ("maturity", ctypes.c_float),
        ("reinforcement_ratio", ctypes.c_float),
    ]


# Opaque pointers
_CrRuntime = ctypes.c_void_p
_CrState = ctypes.c_void_p

# =============================================================================
# Python Data Classes (Pythonic interface)
# =============================================================================

@dataclass
class MindConfig:
    """Configuration for MIND runtime."""
    embedding_dim: int
    max_memory_slots: int
    initial_plasticity: float = 1.0


@dataclass
class Hint:
    """Query result with confidence."""
    vector: Optional[List[float]]
    dim: int
    confidence: float


@dataclass
class Plasticity:
    """Basic epistemic state."""
    plasticity: float
    stability: float
    age: float


@dataclass
class Temporal:
    """Rich temporal awareness."""
    age: float
    plasticity: float
    velocity: float
    maturity: float
    last_reinforcement_age: float
    time_since_reinforcement: float
    total_updates: int
    total_reinforcements: int


@dataclass
class Calibration:
    """S2S calibration signal."""
    age: float
    plasticity: float
    velocity: float
    maturity: float
    reinforcement_ratio: float


# =============================================================================
# Library Loading
# =============================================================================

_lib: Optional[ctypes.CDLL] = None


def _find_library() -> Optional[str]:
    """Find libmind in common locations."""
    # Check environment variable first
    if "MIND_LIB_PATH" in os.environ:
        return os.environ["MIND_LIB_PATH"]

    # Common library names by platform
    if sys.platform == "darwin":
        names = ["libmind.dylib", "libmind.a"]
    elif sys.platform == "win32":
        names = ["mind.dll", "libmind.dll"]
    else:
        names = ["libmind.so", "libmind.a"]

    # Search paths
    search_paths = [
        Path(__file__).parent.parent.parent.parent.parent.parent / "build",  # Relative to repo
        Path.cwd() / "build",
        Path.cwd(),
        Path("/usr/local/lib"),
        Path("/usr/lib"),
    ]

    for path in search_paths:
        for name in names:
            full_path = path / name
            if full_path.exists():
                return str(full_path)

    # Try system library path
    found = ctypes.util.find_library("mind")
    if found:
        return found

    return None


def load_library(path: Optional[str] = None) -> ctypes.CDLL:
    """
    Load the MIND shared library.

    Args:
        path: Optional explicit path to libmind. If not provided,
              searches common locations.

    Returns:
        Loaded ctypes CDLL object.

    Raises:
        OSError: If library cannot be found or loaded.
    """
    global _lib

    if _lib is not None and path is None:
        return _lib

    lib_path = path or _find_library()
    if lib_path is None:
        raise OSError(
            "Cannot find libmind. Set MIND_LIB_PATH environment variable "
            "or pass path explicitly to load_library()."
        )

    lib = ctypes.CDLL(lib_path)
    _setup_functions(lib)

    if path is None:
        _lib = lib

    return lib


def _setup_functions(lib: ctypes.CDLL) -> None:
    """Set up function signatures for type safety."""

    # cr_version
    lib.cr_version.argtypes = []
    lib.cr_version.restype = ctypes.c_char_p

    # cr_runtime_create
    lib.cr_runtime_create.argtypes = [ctypes.POINTER(_CrConfig)]
    lib.cr_runtime_create.restype = _CrRuntime

    # cr_runtime_destroy
    lib.cr_runtime_destroy.argtypes = [_CrRuntime]
    lib.cr_runtime_destroy.restype = None

    # cr_runtime_config
    lib.cr_runtime_config.argtypes = [_CrRuntime, ctypes.POINTER(_CrConfig)]
    lib.cr_runtime_config.restype = ctypes.c_int

    # cr_state_create
    lib.cr_state_create.argtypes = [_CrRuntime]
    lib.cr_state_create.restype = _CrState

    # cr_state_destroy
    lib.cr_state_destroy.argtypes = [_CrState]
    lib.cr_state_destroy.restype = None

    # cr_state_reset
    lib.cr_state_reset.argtypes = [_CrState]
    lib.cr_state_reset.restype = None

    # cr_state_slot_count
    lib.cr_state_slot_count.argtypes = [_CrState]
    lib.cr_state_slot_count.restype = ctypes.c_int

    # cr_state_update
    lib.cr_state_update.argtypes = [
        _CrState,
        ctypes.POINTER(ctypes.c_float),
        ctypes.c_int,
        ctypes.c_float,
    ]
    lib.cr_state_update.restype = ctypes.c_int

    # cr_state_query
    lib.cr_state_query.argtypes = [
        _CrState,
        ctypes.POINTER(ctypes.c_float),
        ctypes.c_int,
        ctypes.POINTER(_CrHint),
    ]
    lib.cr_state_query.restype = ctypes.c_int

    # cr_state_plasticity
    lib.cr_state_plasticity.argtypes = [_CrState, ctypes.POINTER(_CrPlasticity)]
    lib.cr_state_plasticity.restype = ctypes.c_int

    # cr_state_temporal
    lib.cr_state_temporal.argtypes = [_CrState, ctypes.POINTER(_CrTemporal)]
    lib.cr_state_temporal.restype = ctypes.c_int

    # cr_state_calibration
    lib.cr_state_calibration.argtypes = [_CrState, ctypes.POINTER(_CrCalibration)]
    lib.cr_state_calibration.restype = ctypes.c_int

    # cr_state_save
    lib.cr_state_save.argtypes = [_CrState, ctypes.c_char_p]
    lib.cr_state_save.restype = ctypes.c_int

    # cr_state_load
    lib.cr_state_load.argtypes = [_CrState, ctypes.c_char_p]
    lib.cr_state_load.restype = ctypes.c_int


# =============================================================================
# MindState Class
# =============================================================================

class MindState:
    """
    MIND cognitive state.

    Accumulates experience from vector embeddings and provides
    hints with derived confidence.

    Example:
        >>> state = MindState(dim=768, slots=128)
        >>> state.update([0.1, 0.2, ...], delta_t=1.0)
        >>> hint = state.query([0.1, 0.2, ...])
        >>> print(f"Confidence: {hint.confidence}")
    """

    def __init__(
        self,
        dim: int,
        slots: int,
        initial_plasticity: float = 1.0,
        lib_path: Optional[str] = None,
    ):
        """
        Create a new MIND state.

        Args:
            dim: Embedding dimension.
            slots: Maximum memory slots.
            initial_plasticity: Starting plasticity (default 1.0).
            lib_path: Optional path to libmind.
        """
        self._lib = load_library(lib_path)
        self._dim = dim

        # Create config
        config = _CrConfig(
            embedding_dim=dim,
            max_memory_slots=slots,
            initial_plasticity=initial_plasticity,
        )

        # Create runtime
        self._runtime = self._lib.cr_runtime_create(ctypes.byref(config))
        if not self._runtime:
            raise MemoryError("Failed to create MIND runtime")

        # Create state
        self._state = self._lib.cr_state_create(self._runtime)
        if not self._state:
            self._lib.cr_runtime_destroy(self._runtime)
            raise MemoryError("Failed to create MIND state")

    def __del__(self):
        """Clean up resources."""
        if hasattr(self, "_state") and self._state:
            self._lib.cr_state_destroy(self._state)
            self._state = None
        if hasattr(self, "_runtime") and self._runtime:
            self._lib.cr_runtime_destroy(self._runtime)
            self._runtime = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.__del__()
        return False

    @property
    def dim(self) -> int:
        """Embedding dimension."""
        return self._dim

    @property
    def slot_count(self) -> int:
        """Number of occupied memory slots."""
        return self._lib.cr_state_slot_count(self._state)

    def _to_float_array(
        self,
        embedding: Union[List[float], "numpy.ndarray"]
    ) -> ctypes.Array:
        """Convert embedding to ctypes float array."""
        # Handle numpy arrays if present
        if hasattr(embedding, "ctypes"):
            # numpy array - use its ctypes interface
            return embedding.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

        # Regular Python list
        arr = (ctypes.c_float * len(embedding))(*embedding)
        return arr

    def update(
        self,
        embedding: Union[List[float], "numpy.ndarray"],
        delta_t: float = 1.0,
    ) -> None:
        """
        Update state with new experience.

        Args:
            embedding: Vector embedding (list or numpy array).
            delta_t: Time increment (must be positive).

        Raises:
            ValueError: If embedding dimension doesn't match or delta_t <= 0.
            RuntimeError: If update fails.
        """
        if len(embedding) != self._dim:
            raise ValueError(
                f"Embedding dimension {len(embedding)} doesn't match "
                f"configured dimension {self._dim}"
            )
        if delta_t <= 0:
            raise ValueError("delta_t must be positive")

        arr = self._to_float_array(embedding)
        result = self._lib.cr_state_update(
            self._state,
            arr,
            self._dim,
            ctypes.c_float(delta_t),
        )
        if result != 0:
            raise RuntimeError("Failed to update state")

    def query(
        self,
        embedding: Union[List[float], "numpy.ndarray"],
    ) -> Hint:
        """
        Query state for a hint.

        Args:
            embedding: Query embedding.

        Returns:
            Hint with vector reference and confidence.

        Raises:
            ValueError: If embedding dimension doesn't match.
            RuntimeError: If query fails.
        """
        if len(embedding) != self._dim:
            raise ValueError(
                f"Embedding dimension {len(embedding)} doesn't match "
                f"configured dimension {self._dim}"
            )

        arr = self._to_float_array(embedding)
        hint = _CrHint()

        result = self._lib.cr_state_query(
            self._state,
            arr,
            self._dim,
            ctypes.byref(hint),
        )
        if result != 0:
            raise RuntimeError("Failed to query state")

        # Convert to Python
        vector = None
        if hint.vector and hint.dim > 0:
            vector = [hint.vector[i] for i in range(hint.dim)]

        return Hint(
            vector=vector,
            dim=hint.dim,
            confidence=hint.confidence,
        )

    def plasticity(self) -> Plasticity:
        """Get basic epistemic state."""
        p = _CrPlasticity()
        result = self._lib.cr_state_plasticity(self._state, ctypes.byref(p))
        if result != 0:
            raise RuntimeError("Failed to get plasticity")

        return Plasticity(
            plasticity=p.plasticity,
            stability=p.stability,
            age=p.age,
        )

    def temporal(self) -> Temporal:
        """Get rich temporal awareness."""
        t = _CrTemporal()
        result = self._lib.cr_state_temporal(self._state, ctypes.byref(t))
        if result != 0:
            raise RuntimeError("Failed to get temporal state")

        return Temporal(
            age=t.age,
            plasticity=t.plasticity,
            velocity=t.velocity,
            maturity=t.maturity,
            last_reinforcement_age=t.last_reinforcement_age,
            time_since_reinforcement=t.time_since_reinforcement,
            total_updates=t.total_updates,
            total_reinforcements=t.total_reinforcements,
        )

    def calibration(self) -> Calibration:
        """Get S2S calibration signal."""
        c = _CrCalibration()
        result = self._lib.cr_state_calibration(self._state, ctypes.byref(c))
        if result != 0:
            raise RuntimeError("Failed to get calibration")

        return Calibration(
            age=c.age,
            plasticity=c.plasticity,
            velocity=c.velocity,
            maturity=c.maturity,
            reinforcement_ratio=c.reinforcement_ratio,
        )

    def reset(self) -> None:
        """Reset state to initial condition."""
        self._lib.cr_state_reset(self._state)

    def save(self, path: Union[str, Path]) -> None:
        """
        Save state to file.

        Args:
            path: File path.

        Raises:
            RuntimeError: If save fails.
        """
        path_bytes = str(path).encode("utf-8")
        result = self._lib.cr_state_save(self._state, path_bytes)
        if result != 0:
            raise RuntimeError(f"Failed to save state to {path}")

    def load(self, path: Union[str, Path]) -> None:
        """
        Load state from file.

        Args:
            path: File path.

        Raises:
            RuntimeError: If load fails.
            FileNotFoundError: If file doesn't exist.
        """
        path = Path(path)
        if not path.exists():
            raise FileNotFoundError(f"State file not found: {path}")

        path_bytes = str(path).encode("utf-8")
        result = self._lib.cr_state_load(self._state, path_bytes)
        if result != 0:
            raise RuntimeError(f"Failed to load state from {path}")
