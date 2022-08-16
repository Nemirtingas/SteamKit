## NetHook2

NetHook2 is a linux library that is injected into the address space of a running steam process in order to hook into the networking routines of the Steam client. After hooking, NetHook2 will proceed to dump any network messages sent to and received from the Steam server that the client is connected to.

These messages are dumped to file, and can be analyzed further with NetHookAnalyzer, a hex editor, or your own purpose-built tools.

## Compiling

#### Building

1. Execute `SetupDependencies.sh` to automatically acquire and build the zlib and protobuf headers and libraries. You will need CMake for this.
2. Build NetHook with make
3. Behold: a fresh new `NetHook2.so` is born into this world. You can place this library wherever you like, or leave where you built it. You'll need its full file path later when injecting.

#### Updating steammessages_base

1. Download `protoc` for the same version as specified in `SetupDependencies`.
2. Run `.\protoc.exe .\steammessages_base.proto --cpp_out=build`

## Usage

NetHook is capable of self injecting and ejecting from running instances of Steam, so there's no requirement to use a separate loader such as winject.

#### Downloading pre-built binary

As SteamKit2 releases can lag behind Steam changes, the current master branch may contain fixes to NetHook that are not released yet.

You can download latest available build from [GitHub Actions here](https://github.com/SteamRE/SteamKit/actions).  
Click on the latest commit and scroll all the way down to Artifacts section.
There, you can download NetHook and NetHookAnalyzer.

#### Installing dependencies


#### To begin dumping network packets

1. Ensure Steam is running. Additionally, make sure you're prepared for the possibility for Steam to crash. Using NetHook2 isn't an exact science, and sometimes things break.
2. Execute the following in an _elevated_ command prompt: 
```
gdb -p "$(ps -ef | grep '32/steam' | grep -v grep | head -1 | awk '{ print $2 }')"
call (void*)dlopen("<Path To NetHook2.so>", 2)
# Save the return value in notepad or something ($1 = (void *) 0x5778d4f0)
c
```

If all goes well, you should see output in steam logs similar to the following:

```
CCrypto::SymmetricEncryptWithIV = 0x384b84c0
CCrypto::SymmetricDecrypt = 0x384b8290
pGetMessageList = 0x3843c030
pInfos = 0x38874dc0
numMessages = 502
Dumped emsg list! (502 messages)
Detoured SymmetricEncryptWithIV!
Detoured SymmetricDecrypt!
```

If instead you see a failure message or your Steam process crashes, it's possible that an update to Steam may have broken one of NetHook2's binary signature scans and they will require updating. If you're on the beta branch of Steam, try using the non-beta version and vice versa. Additionally check to see if there's any recent NetHook2 related commits on our `steamclient-beta` branch.

If nothing seems to work, feel free to file an issue or hop [on IRC](https://github.com/SteamRE/SteamKit/wiki#contact) and let us know!

Provided everything successfully injected and hooked, NetHook2 will now begin dumping every message to file. You can locate the dumps in your Steam install, under the `nethook` directory. The directories are numbered with the unix time that dumping began.

#### To stop dumping packets

Simply execute in your gdb instance 
```
<ctrl+c>
# 0x5778d4f0 is the value you saved earlier when you injected the library
(void)dlclose(0x5778d4f0)
q
```

NetHook2 will eject itself from the running Steam instance.

#### Viewing the dumped packets

Packet dumps are written to `nethook/<timestamp>` folder inside of your Steam installation.  
`<timestamp>` indicates the time NetHook was injected.

Open `NetHookAnalyzer2.exe` and then File->Open, it should automatically default to the latest folder created by NetHook.
