The +1 stager demo of h4x
=========================

To run (requires Linux):
  * `git clone`
  * `git submodule init`
  * `git submodule update`
  * `make`
  * Use `objdump -x test` to find the `_start_c` symbol
  * Copy the offset into `loader.c:37`
  * `make` (again)
  * `./loader`
  * You should see `Hello, world!`


TODO
----

* Convert the C code in `loader.c` into shell code (generate with the metadata from the ELF file)
* Integrate with mettle
* ???
* Profit
