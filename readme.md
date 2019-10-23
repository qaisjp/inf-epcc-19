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

### `#pragma omp for` (work sharing!)

Parallel loop directives are **very important** because they are the most common source of parallelism in most code.

A parallel loop divides iterations of the loop between threads — a loop directive indicates that work should be
**shared out between threads, instead of replicated**.

There is a sync point at the end of the loop: all threads must finish their iterations before any thread can proceed.

```
#pragma omp for [clauses]
```

This `for` directive accepts `private`, `firstprivate` and `reduction` clauses which refer to the scope of the loop.
The parallel loop index is `private` by default!

With no additional clauses, the `for` directive will partition iterations as equally as possible between threads.

This is implementation dependent and there is still some ambiguity.
For example, take 7 iterations & 3 threads. You can partition as `3+3+1` or `3+2+2`.

**C++ restrictions**

It has to have **determinable trip count**. Of this form:

```
for (somevar = a; somevar LOGICAL_OP b; INCR_EXP)
```

- `LOGICAL_OP` is oneof `<, <=, >, >=`
- `INCR_EXP` is `somevar = somevar +/- incr` (or semantic equivalents, like `somevar++`)
- You cannot modify `somevar` within the loop body

**Example**

```
#pragma omp parallel
{
    #pragma omp for
    for (int i = 0; i < n; i++)
        b[i] = (a[i] * a[i-1]) * 0.5;
}
```

### `#pragma omp parallel for`

The above construct (`omp parallel` > `omp for`) is so common that you can do this instead:

```
#pragma omp parallel for [clauses]
[for loop]
```

This `parallel for` directive can take **all clauses** available for the `parallel` directive.
This directive is **NOT THE SAME** as the `for` directive or the `parallel` directive.

### `#pragma omp schedule(kind,`[`,chunksize`]`)`

This clause gives a variety of options for specifying which loops iterations are executed by which thread.

- `kind` is oneof `static, dynamic, guided, auto,` or `runtime`
- `chunksize` is an integer expression with positive value

**`static` schedule** (see `slides/L04-worksharing.pdf#page=11` and page 12)

- If `chunksize` **IS NOT** specified
    - the iteration space is divided into (approximately) equal chunks
    - one chunk is assigned to each thread in order (**block** schedule)
- If `chunksize` **IS** specified
    - the iteration space is divided into chunks, each of `chunksize` iterations
    - the chunks are assigned cyclically to each thread in order (**block cyclic** schedule)

**`dynamic` schedule** (see `slides/L04-worksharing.pdf#page=13` and page 15)

- If `chunksize` **IS** specified
    - the iteration space is divided into chunks of size `chunksize`
    - chunks are assigned to threads on a first-come-first-served basis
- If `chunksize` **IS NOT** specified, it defaults to `1`

As a thread finishes a chunk, it is assigned the next chunk in the list.

**`guided` schedule** (see `slides/L04-worksharing.pdf#page=14` and page 15)

Similar `dynamic`, but chunks start off large and get smaller exponentially.

The size of the next chunk is proportional to `remainingIterations / threadCount`

- If `chunksize` **IS** specified
    - it represents the minimum size of the chunks
- If `chunksize` **IS NOT** specified, it defaults to `1`

As a thread finishes a chunk, it is assigned the next chunk in the list.

**`auto` schedule** (see `slides/L04-worksharing.pdf#page=16`)

This lets the runtime have full freedom to choose its own assignment of iterations to threads.

If the parallel loop is executed many times, the runtime can evolve
a good schedule which has good load balance and low overheads

**Choosing a schedule**

- `static`
    - (default chunksize) best for load balanced loops - least overhead
    - (`n` chunksize) good for loops with mild or smooth load imbalance, but can induce overheads
- `dynamic`
    - useful if iterations have widely varying loads, but ruins data locality
- `guided`
    - often less expensive than `dynamic`
    - beware of loops where the first iterations are the most expensive!
- `auto`
    - may be useful if the loop is executed many times over

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
