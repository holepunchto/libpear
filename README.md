# libpear

Native utilities for Pear applications.

## API

### pear_launch
The primary function for launching a Pear appling. It initializes the internal Pear runtime and manages appling execution.

```c
pear_launch (
    int argc, 
    char *argv[], 
    pear_key_t key, 
    const char *name
)
```

- argc: The number of command-line arguments.
- argv: An array of command-line arguments.
- key: The unique appling key.
- name: The name of the Pear appling.


## License

Apache-2.0
