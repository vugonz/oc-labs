## Tests
To run the tests change into `test` directory and just run `make test`.

Tests with prefix `fg` are fine grained tests. In these tests every memory
access is tested to see if it matches the expected time penalties, tests not
fine grained simply take the total time taken and compare it to the expected
total.
