## Directives
### Multi-line directives

End the line with a `\`, like so

```c
#pragma omp parallel default(none) \
        private(somevar)
```

### `#pragma omp parallel`

```c
apple();                 | [apple]
#pragma omp parallel
{
  blueberry();           | [blueberry][blueberry][blueberry][blueberry]
}
cranberry();             | [cranberry]
```

Additional information can be specified through clauses (comma separated or space separated)

```
#pragma omp parallel [clauses]
```

## Useful Functions
`#include <omp.h>` first.

### `int omp_get_num_threads();`

Find out number of threads being used.

- Inside parallel region, returns number of threads being used
- Outside parallel regoin, returns `1`

### `int omp_get_thread_num();`

Find out number of existing thread.

- Returns int between `0` and `omp_get_num_threads() - 1`


## Shared and private variables

Variables can be:
- **shared**: all threads see _same copy_ - it reads/writes to the exact same variable
- **private**: each thread has _own copy_, invisible to other threads - can only be read/written by its own thread

You can set a variable "type" (???) using the following clauses
- `shared(`[`list`]`)`
- `private(`[`list`]`)`
- `default(`[`shared|none`]`)` - by default, the default is shared. always use `default(none)`!

How they affect the parallel region:
- On entry to a parallel region, **private variables are uninitialised**!
- Variables declared inside the region are **automatically private**
- After the region ends, **original variable** is **unaffected** by changes to private copies

Note: _private objects are created using the default constructor_
