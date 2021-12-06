# remotelockd
remotely suspend your pc. uses one-time tokens for authenticating requests.

the security of this is pathetic don't use it.

> depends: libnotify, systemd

## how to

### on pc
* define `PORT` in `rlockd.c`
* run `make`
* `./rlockd -s`

> daemon exits after sucessfully suspending, i.e. needs to be restarted

### on mobile
> in termux
* adjust `IP` and `PORT` in `suspendpc.sh`
* `./suspendpc.sh <TOKEN>`

## other
* the generated tokens are numeric, they can be brute-forced
* i'd advise against running this on anything other than your (trusted) local home network
* license: gpl3

> beware, valgrind reports some memory leaks
