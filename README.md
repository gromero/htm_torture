# Test Framework to run certain workloads inside a transactional memory.

This test runs on many threads, and check if the VMX states are being recovered properly. I.e, the state of the VMX states are the same before and after the transaction.

## Supported OSes:
 * Linux
 * FreeBSD (WIP)
