# Changelog

## 2026-06-28

### Changed

- Established `include/Dline/core.h` as the public DLL interface boundary.
- Added a shared `DlineResult` error type and static error descriptions.
- Changed internal allocation helpers to return `NULL` instead of terminating
  the host process on out-of-memory conditions.
- Completed buffer initialization, destruction, reservation, and append
  operations with explicit error propagation and capacity overflow checks.
- Kept UTF-8 and buffer headers private to the core implementation.
- Reworked CMake so Dline can be consumed through the `Dline::Core` target and
  tests are disabled by default when Dline is included as a subproject.
- Added deterministic UTF-8 boundary and buffer lifecycle tests registered
  with CTest.
- Added a public API linkage test against the generated dynamic library.

### Fixed

- Removed the malformed global include path containing a trailing `$`.
- Corrected mismatched buffer declaration and implementation names.
- Corrected the `dline_strdup` implementation name.
- Removed duplicate and inconsistent result types from internal headers.
