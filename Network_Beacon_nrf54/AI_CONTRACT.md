# AI Contract

Rules for AI-assisted work in this repository.

## Non-Negotiables

- Read the relevant owner files before editing.
- Keep domain logic in the domain that owns it.
- Do not add one-off hacks in startup code, callbacks, or storage wrappers.
- Do not scatter protocol constants or magic command IDs.
- Preserve unrelated user changes.
- Document non-obvious architectural choices in `DECISIONS.md`.



## Change Rules

- Add new parameters to the correct protocol group and implement them in the owning domain.
- Validate command values before applying or saving them.
- Persistent defaults, reset behavior, and validation belong with the owning domain.
- If a change affects advertised data or exported data format, treat it as a compatibility decision and record it.
- Keep callbacks short; move policy into named helpers or owner-domain functions.

## Test Rules

- For code changes, run a build or clearly state why it was not run:

```powershell
west build -b nrf54l15dk/nrf54l15/cpuapp/ns -d build_debug --sysbuild .
```

- For BLE, NUS, storage, commands, or contact behavior, record the hardware/manual checks performed.
- Do not claim tests passed unless they were actually run.
