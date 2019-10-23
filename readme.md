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


## Clauses

### `shared`, `private`, and `default`

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

### `firstprivate`

Because **private variables are uninitialised on entry to a parallel region**, sometimes you might need to
initialise them. In C++ the default copy constructor is called to create and initialise the new object.

Use cases for this are uncommon, but can be done by `firstprivate(`[`list`]`)` clause.

**Example** (from [Stack Overflow](https://stackoverflow.com/a/15309556/1517394))

```c
int i = 10;

#pragma omp parallel firstprivate(i)
{
        printf("thread %d: i = %d\n", omp_get_thread_num(), i);
        i = 1000 + omp_get_thread_num();
}

printf("i = %d\n", i);
```

The above code prints out:

> thread 2: i = 10
> thread 0: i = 10
> thread 3: i = 10
> thread 1: i = 10
> i = 10

However, if you modify the directive line to use `private` instead (`#pragma omp parallel firstprivate(i)`), you'll get:

> thread 2: i = <random>
> thread 1: i = <random>
> thread 0: i = <random>
> thread 3: i = <random>
> i = 10

So, **firstprivate will initialise the variable to the original value of the variable**.

### `reduction(`_`op`_`:`_`list`_`)`

A _reduction_ produces a **single value** from associative operations.

**Examples of operations**: addition, multiplication, max, min, and, or

Each thread reduces into a private copy, and then all those thread-specific results are reduces into a final result.

**Example** ([jakascorner.com](http://jakascorner.com/blog/2016/06/omp-for-reduction.html#implementation))

```c
sum = 0;
#pragma omp parallel for shared(sum, a) reduction(+: sum)
for (auto i = 0; i < 9; i++)
{
    sum += a[i]
}
```

Each thread performs a chunk of the computation:
- thread 1: `sum1 = a[0] + a[1] + a[2]`
- thread 2: `sum2 = a[3] + a[4] + a[5]`
- thread 3: `sum3 = a[6] + a[7] + a[8]`

And then, at the end, it reduces these operations **using the variable and operator you provide**: `sum = sum1 + sum2 + sum3`

![Example of reduction](http://jakascorner.com/pics/reduction_clause.png)
