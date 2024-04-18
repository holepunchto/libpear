# libpear

Native utilities for Pear applications.

## API

See [`include/pear.h`](include/pear.h) for the public API.

#### `pear_launch(argc, argv, key, name)`

The primary function for launching a Pear appling. It initializes the internal Pear runtime and manages appling execution.

```c
pear_launch (
  int argc, 
  char *argv[], 
  pear_key_t key, 
  const char *name
)
```

## License

Apache-2.0
