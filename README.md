## What
Just having fun expermineting with chains and stuff

### The stuff
```shell
$ git clone ...
$ cd stupidchain
$ make clean && make && build/server
```

(In a seperate shell)
```shell
$ cd stupidchain
$ build/client
Enter command:
> D
Connected to blockchain server at 127.0.0.1:8080
Requesting blockchain dump...
Server reports 1 blocks.
Receiving block 0 (length 88 bytes)...
--- Received Block 0 ---
0|1748727856|WHAT I BRING IS LIGHT
WHAT I BRING IS A STAR
WHAT I BRING IS
AN ANCIENT SEA
----------------------
Server final response: Blockchain dump complete.
> 
```

### ToDo
- Hashing function/mining
- Multi transactional blocks
- Better client interface
- Ledger pull/push
- Consensus mech