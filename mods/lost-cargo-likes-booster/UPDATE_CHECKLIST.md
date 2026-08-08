# Update checklist

Do not use the mod after a DS2 executable update until revalidated.

1. Confirm the DS2.exe PE timestamp and SizeOfImage still match the source guards.
2. Verify the Lost Cargo writer `DS2.exe+0x1E1335D` and caller `+0x1E14C74` with a read-only debugger trace.
3. Verify the Connection-update AOB remains unique.
4. Verify a regular order is excluded.
5. Verify one Lost Cargo return receives the configured Like bonus only once.
6. Verify the matched Facility Connection Points increase by the same configured amount.
7. Verify normal DS2 star progression occurs without a direct level override.
