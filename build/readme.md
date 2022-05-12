# Build Scripts

> `cmake` isn't set up yet, thus `build.sh` doesn't do something useful for now.

The scripts must have this directory as working directory.

`dep_*` files are dependencies of the scripts and should not be called directly.



## `*.sh` Scripts

They return __0__ on success and __1__ on error.
Eexcept the `pack_*` scripts don't.



---

---

### files to edit on version update

- `build/dep_vstr.txt`
- `include/rpihal/defs.h`
