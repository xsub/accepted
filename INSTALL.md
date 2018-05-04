
# Installation guidoe for accepted

## Brief
Installation for accepted/rejected duo is trivial as it follows the autotools logic.

## Steps:

autoreconf --install
./configure
make install 

### Notes:
- Use `./configure --prefix=$TARGET_DIR` to specify your own install dir.
- use sudo if configured with no prefix, this is for /bin installation.
