# emacs-zstd

[zstd](https://github.com/facebook/zstd) binding of Emacs Lisp

## Interfaces

### `(zstd-compress string &optional level) -> vector`

Compress string

### `(zstd-decompress vector) -> vector`

Decompress vector which contains bytes