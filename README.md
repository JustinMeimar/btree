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
|-----------|------------------|------------------|-----------------------|------------------------------------|------------------------------------|
| 100       | 0.21084          | 0.125441         | 0.019177              | 0.002108                           | 0.001254                           |
| 1K        | 2.87546          | 1.66704          | 0.033443              | 0.002875                           | 0.001667                           |
| 100K      | 400.561          | 251.863          | 7.09349               | 0.004006                           | 0.002519                           |
| 1M        | 4915.6           | 2942.86          | 89.8243               | 0.004916                           | 0.002943                           |
| 10M       | 63631.1          | 33006.6          | 987.689               | 0.006363                           | 0.003301                           |
| 100M      | 827942             | 477438 |  