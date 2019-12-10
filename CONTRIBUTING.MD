# Contributing

libe4 is maintained by [Teserakt AG](https://teserakt.io) and its team:

* [@diagprov](https://github.com/diagprov) (Antony Vennard)
* [@veorq](https://github.com/veorq) (JP Aumasson)

We welcome and encourage third-party contributions to libe4, be it reports of 
issues encountered while using the software, suggestions of new features, or 
proposals of patches.

## Bug reports

Bugs, problems, and feature requests should be reported on 
[GitHub Issues](https://github.com/teserakt-io/e4go/issues).

If you report a bug, please:

* Check that it's not already reported in the 
  [GitHub Issues](https://github.com/teserakt-io/e4go/issues).
* Provide information to help us diagnose and ideally reproduce the bug.

We appreciate feature requests, however we cannot guarantee that all the 
features requested will be added to e4go.

## Patches

We encourage you to fix a bug or implement a new feature via a 
[GitHub Pull request](https://github.com/teserakt-io/e4go/pulls), preferably 
after creating a related issue and referring it in the PR.

If you contribute code and submit a patch, please note the following design 
philosophy of the code, which must be adhered to. Unfortunately, we cannot 
accept patches that deviate from these rules:

* The code must run freestanding. Certain parts of libc may not be available.
* C89 is targeted for maximum portability. 
* Any kind of dynamic allocation is prohibited. This includes the use of 
  `malloc`, `calloc`, and some more surprising functions such as `strdup`. 
  Some security and safety testing profiles in (for example) the automotive 
  industry require this. In addition, an allocator may not be available.
* Host unit tests may deviate from the above rules.
* Device tests (tests designed to be deployed on devices) may vary.
* Pull requests should target the `develop` branch.
* Please follow the C coding conventions that exist. If in doubt, run 
  `make format` before sending your patch.

Also please make sure to create new unit tests covering your code additions. 
You can execute the tests by running:

```bash
CONF=yourmode make test
```

All third-party contributions will be recognized in the list of contributors.

## House rules

When posting on discussion threads, please be respectful and civil, avoid 
(passive-)agressive tone, and try to communicate clearly and succinctly. 
This is usually better for everyone :-)
