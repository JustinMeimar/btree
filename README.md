# B+ Tree Implementation

Build: 
```shell
make
```

Run Tests:
Randomized or Sequential test cases can be generated with `gen.py` and run with `run-tests.py`.
```shell
python3 tests/gen.py <n> <output_file_name> -r
python3 tests/run-tests.py
```

# Benchmarks
Performed on Intel i5-9400F with 32GB RAM

- `Insert Time`: Total time to insert `Data Size` elements into the tree from a random non-repeating distribution.
- `Look Up Time`:  Total time to look up each inserted element after the tree is completely filled.
- `Full-Range Query`: Time to traverse the entire tree by adjacent leaf node pointers. In practice used for ranged queries. 
- `Avg Insert`: Insert time normalized by `Data Size`
- `Avg Lookup`: Lookup time normalized by `Data Size`

| Data Size | Insert Time (ms) | Lookup Time (ms) | Full-Range Query (ms) | Avg Insert (ms/insert) | Avg Lookup (ms/lookup) |
|-----------|------------------|------------------|-----------------------|------------------------|------------------------|
| 100       | 0.21084          | 0.12544          | 0.01918               | 0.0021080              | 0.0012540              |
| 1K        | 2.87546          | 1.66704          | 0.03344               | 0.0028750              | 0.0016670              |
| 100K      | 400.56100        | 251.86300        | 7.09349               | 0.0040060              | 0.0025190              |
| 1M        | 4915.60000       | 2942.86000       | 89.82430              | 0.0049160              | 0.0029430              |
| 10M       | 63631.10000      | 33006.60000      | 987.68900             | 0.0063630              | 0.0033010              |
| 100M      | 827942.00000     | 477438.00000     | 8153.67000            | 0.0082794              | 0.0047744              |