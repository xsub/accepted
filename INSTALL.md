
# Installation guide for **Accepted**

## Brief

Installation of **Accepted** package equals to installation `accepted` & `rejected` duo (excluding manual that is). It is trivial as follows the **GNU Autotools** project guideliness and thus supports the well known `configure` && `make install` combo. 

## Installation steps

```
autoreconf --install
./configure
make install 
```

### Installation notes

- Use `./configure --prefix=$TARGET_DIR` to specify your own install dir.
- use `sudo` if configured with no prefix, this is allowing install to `/bin`.
