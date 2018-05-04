
# Installation guide for *accepted*

## Brief

Installation for accepted/rejected duo is trivial as it follows the autotools logic.

## Installation steps

```
autoreconf --install
./configure
make install 
```

### Installation notes
- Use `./configure --prefix=$TARGET_DIR` to specify your own install dir.
- use `sudo` if configured with no prefix, this is allowing install to `/bin`.
